// Part of ImGui Bundle - MIT License - Copyright (c) 2022-2024 Pascal Thomet - https://github.com/pthom/imgui_bundle
#ifdef IMGUI_BUNDLE_WITH_IMGUIZMO
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/optional.h>
#include <nanobind/make_iterator.h>
#include <nanobind/trampoline.h>
#include <nanobind/ndarray.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "ImGuizmoPure/ImCurveEditPure.h"
#include "ImGuizmoPure/ImGradientPure.h"
#include "ImGuizmoPure/ImZoomSliderPure.h"
#include "ImGuizmoPure/ImGuizmoPure.h"


namespace nb = nanobind;
using namespace ImGuizmo;


// ============================================================================
// Hairy conversions between MatrixXX and numpy arrays, with shared memory
// (inspired by my previous work on cvnp (https://github.com/pthom/cvnp)
// ============================================================================

// ----------------------------------------------------------------------------
// Part 1 : Conversion utilities
// ----------------------------------------------------------------------------
namespace matrix_to_numpy
{
    template<int N>
    std::vector<size_t> matrix_shape()
    {
        // we will transcribe a Matrix16 into a 4x4 matrix on python side
        // But the others will stay flat
        if (N == 3)
            return {3};
        else if (N == 6)
            return {6};
        else if (N == 16)
            return {4, 4};
        else
            throw std::runtime_error("pybind_imguizmo.cpp: matrix_to_numpy::matrix_shape => bad N");
    }

    template<int N>
    std::vector<int64_t> matrix_strides()
    {
        nb::ssize_t s = sizeof(float);
        if ((N == 3) || (N == 6))
            return {s};
        else if (N == 16)
            return { 4 * s, s };
        else
            throw std::runtime_error("pybind_imguizmo.cpp: matrix_to_numpy::matrix_strides => bad N");
    }

    template<int N>
    nb::ndarray<> matrix_to_nparray(const MatrixFixedSize<N>& m)
    {
        std::vector<size_t> shape = matrix_shape<N>();
        std::vector<int64_t> strides = matrix_strides<N>();

        // old pybind11 code
        // static std::string float_numpy_str = nb::format_descriptor<float>::format();
        // static auto dtype_float = nb::dtype(float_numpy_str);
        // return nb::array(dtype_float, shape, strides, m.values);

        // new nanobind code:
        // ndarray constructor signature:
        //        ndarray(VoidPtr data,
        //            size_t ndim,
        //            const size_t *shape,
        //            handle owner = { },
        //            const int64_t *strides = nullptr,
        //            dlpack::dtype dtype = nanobind::dtype<Scalar>(),
        //            int device_type = DeviceType,
        //            int device_id = 0,
        //            char order = Order)
        nb::ndarray<> a(
            const_cast<void*>(static_cast<const void*>(m.values)), // Ensure non-const void*
            shape.size(), // ndim
            shape.data(), // shape
            {}, // owner
            strides.data(), // strides as initializer_list
            nb::dtype<float>()
        );
        return a;
    }

    template<int N>
    MatrixFixedSize<N> nparray_to_matrix(nb::ndarray<>& a)
    {
        MatrixFixedSize<N> r;

        if (a.itemsize() != sizeof(float))
            throw std::runtime_error("pybind_imguizmo.cpp::nparray_to_matrix / only numpy arrays of type np.float32 are supported!");

        // Check input array type

        // old pybind11 code
        //        if (a.dtype().kind() != nb::format_descriptor<float>::c)
        //            throw std::runtime_error("pybind_imguizmo.cpp::nparray_to_matrix / only numpy arrays of type np.float32 are supported!");

        // new nanobind code:
        if (! (a.dtype().code == static_cast<uint8_t>(nb::dlpack::dtype_code::Float) && a.dtype().bits / 8 == sizeof(float)))
            throw std::runtime_error("pybind_imguizmo.cpp::nparray_to_matrix / only numpy arrays of type np.float32 are supported!");

        // Check input array total length
        if (a.size() != N)
            throw std::runtime_error("pybind_imguizmo.cpp::nparray_to_matrix / bad size!");

        // ...and then copy its values
        const float* np_values_ptr = static_cast<const float*>(a.data());
        for (int i = 0; i < N; ++i)
            r.values[i] = np_values_ptr[i];

        return r;
    }

}

// ----------------------------------------------------------------------------
// Part 1 : Type casters numpy.array <=> MatrixXX
// ----------------------------------------------------------------------------
namespace nanobind
{
    namespace detail
    {
        template<int N>
        struct type_caster<MatrixFixedSize<N>>
        {
            NB_TYPE_CASTER(MatrixFixedSize<N>, const_name("MatrixFixedSize"))

            bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept
            {
                // Check if the source is a numpy.ndarray
                if (!isinstance<ndarray<>>(src))
                {
                    PyErr_WarnFormat(PyExc_Warning, 1, "nanobind: MatrixFixedSize type_caster from_python: expected a numpy.ndarray");
                    return false;
                }

                try
                {
                    auto a = nb::cast<ndarray<>>(src);
                    // Store the conversion into the member
                    // value (of type MatrixFixedSize<N>)
                    value = matrix_to_numpy::nparray_to_matrix<N>(a);
                    return true;
                }
                catch (const std::runtime_error& e) {
                    PyErr_WarnFormat(PyExc_Warning, 1, "nanobind: exception in MatrixFixedSize type_caster from_python: %s", e.what());
                    return false;
                }
            }

            static handle from_cpp(const MatrixFixedSize<N> &m, rv_policy policy, cleanup_list *cleanup) noexcept
            {
                try {
                    ndarray<> a = matrix_to_numpy::matrix_to_nparray<N>(m); // This succeeds
                    // inspired by ndarray.h caster:
                    // We need to call ndarray_export to export a python handle for the ndarray
                    auto r = ndarray_export(
                        a.handle(), // internal array handle
                        nb::numpy::value, // framework (i.e numpy, pytorch, etc)
                        policy,
                        cleanup);
                    return r;
                }
                catch (const std::runtime_error& e) {
                    PyErr_WarnFormat(PyExc_Warning, 1, "nanobind: exception in MatrixFixedSize type_caster from_cpp: %s", e.what());
                    return {};
                }
            }
        };
    }
}


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// <litgen_glue_code>  // Autogenerated code below! Do not edit!

namespace ImCurveEdit {
// helper type to enable overriding virtual methods in python
class Delegate_trampoline : public Delegate
{
public:
    using Delegate::Delegate;

    size_t GetCurveCount() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImCurveEdit::Delegate, // parent class
            "get_curve_count", // function name (python)
            GetCurveCount // function name (c++)
        );
    }
    bool IsVisible(size_t param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            bool, // return type
            ImCurveEdit::Delegate, // parent class
            "is_visible", // function name (python)
            IsVisible, // function name (c++)
            param_0 // params
        );
    }
    ImCurveEdit::CurveType GetCurveType(size_t param_0) const override
    {
        PYBIND11_OVERRIDE_NAME(
            ImCurveEdit::CurveType, // return type
            ImCurveEdit::Delegate, // parent class
            "get_curve_type", // function name (python)
            GetCurveType, // function name (c++)
            param_0 // params
        );
    }
    ImVec2 & GetMin() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::Delegate, // parent class
            "get_min", // function name (python)
            GetMin // function name (c++)
        );
    }
    ImVec2 & GetMax() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::Delegate, // parent class
            "get_max", // function name (python)
            GetMax // function name (c++)
        );
    }
    size_t GetPointCount(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImCurveEdit::Delegate, // parent class
            "get_point_count", // function name (python)
            GetPointCount, // function name (c++)
            curveIndex // params
        );
    }
    uint32_t GetCurveColor(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            uint32_t, // return type
            ImCurveEdit::Delegate, // parent class
            "get_curve_color", // function name (python)
            GetCurveColor, // function name (c++)
            curveIndex // params
        );
    }
    ImVec2 * GetPoints(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 *, // return type
            ImCurveEdit::Delegate, // parent class
            "get_points", // function name (python)
            GetPoints, // function name (c++)
            curveIndex // params
        );
    }
    int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImCurveEdit::Delegate, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            curveIndex, pointIndex, value // params
        );
    }
    void AddPoint(size_t curveIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImCurveEdit::Delegate, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            curveIndex, value // params
        );
    }
    unsigned int GetBackgroundColor() override
    {
        PYBIND11_OVERRIDE_NAME(
            unsigned int, // return type
            ImCurveEdit::Delegate, // parent class
            "get_background_color", // function name (python)
            GetBackgroundColor // function name (c++)
        );
    }
    void BeginEdit(int param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::Delegate, // parent class
            "begin_edit", // function name (python)
            BeginEdit, // function name (c++)
            param_0 // params
        );
    }
    void EndEdit() override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::Delegate, // parent class
            "end_edit", // function name (python)
            EndEdit // function name (c++)
        );
    }
};
}  // namespace ImCurveEdit

namespace ImCurveEdit {
// helper type to enable overriding virtual methods in python
class DelegatePure_trampoline : public DelegatePure
{
public:
    using DelegatePure::DelegatePure;

    size_t GetPointCount(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_NAME(
            size_t, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_point_count", // function name (python)
            GetPointCount, // function name (c++)
            curveIndex // params
        );
    }
    ImVec2 * GetPoints(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_NAME(
            ImVec2 *, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_points", // function name (python)
            GetPoints, // function name (c++)
            curveIndex // params
        );
    }
    std::vector<ImVec2> & GetPointsList(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            std::vector<ImVec2> &, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_points_list", // function name (python)
            GetPointsList, // function name (c++)
            curveIndex // params
        );
    }
    size_t GetCurveCount() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_curve_count", // function name (python)
            GetCurveCount // function name (c++)
        );
    }
    bool IsVisible(size_t param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            bool, // return type
            ImCurveEdit::DelegatePure, // parent class
            "is_visible", // function name (python)
            IsVisible, // function name (c++)
            param_0 // params
        );
    }
    ImCurveEdit::CurveType GetCurveType(size_t param_0) const override
    {
        PYBIND11_OVERRIDE_NAME(
            ImCurveEdit::CurveType, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_curve_type", // function name (python)
            GetCurveType, // function name (c++)
            param_0 // params
        );
    }
    ImVec2 & GetMin() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_min", // function name (python)
            GetMin // function name (c++)
        );
    }
    ImVec2 & GetMax() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec2 &, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_max", // function name (python)
            GetMax // function name (c++)
        );
    }
    uint32_t GetCurveColor(size_t curveIndex) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            uint32_t, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_curve_color", // function name (python)
            GetCurveColor, // function name (c++)
            curveIndex // params
        );
    }
    int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImCurveEdit::DelegatePure, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            curveIndex, pointIndex, value // params
        );
    }
    void AddPoint(size_t curveIndex, ImVec2 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImCurveEdit::DelegatePure, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            curveIndex, value // params
        );
    }
    unsigned int GetBackgroundColor() override
    {
        PYBIND11_OVERRIDE_NAME(
            unsigned int, // return type
            ImCurveEdit::DelegatePure, // parent class
            "get_background_color", // function name (python)
            GetBackgroundColor // function name (c++)
        );
    }
    void BeginEdit(int param_0) override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::DelegatePure, // parent class
            "begin_edit", // function name (python)
            BeginEdit, // function name (c++)
            param_0 // params
        );
    }
    void EndEdit() override
    {
        PYBIND11_OVERRIDE_NAME(
            void, // return type
            ImCurveEdit::DelegatePure, // parent class
            "end_edit", // function name (python)
            EndEdit // function name (c++)
        );
    }
};
}  // namespace ImCurveEdit

namespace ImGradient {
// helper type to enable overriding virtual methods in python
class Delegate_trampoline : public Delegate
{
public:
    using Delegate::Delegate;

    size_t GetPointCount() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            size_t, // return type
            ImGradient::Delegate, // parent class
            "get_point_count", // function name (python)
            GetPointCount // function name (c++)
        );
    }
    ImVec4 * GetPoints() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec4 *, // return type
            ImGradient::Delegate, // parent class
            "get_points", // function name (python)
            GetPoints // function name (c++)
        );
    }
    int EditPoint(int pointIndex, ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImGradient::Delegate, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            pointIndex, value // params
        );
    }
    ImVec4 GetPoint(float t) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec4, // return type
            ImGradient::Delegate, // parent class
            "get_point", // function name (python)
            GetPoint, // function name (c++)
            t // params
        );
    }
    void AddPoint(ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImGradient::Delegate, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            value // params
        );
    }
};
}  // namespace ImGradient

namespace ImGradient {
// helper type to enable overriding virtual methods in python
class DelegateStl_trampoline : public DelegateStl
{
public:
    using DelegateStl::DelegateStl;

    size_t GetPointCount() override
    {
        PYBIND11_OVERRIDE_NAME(
            size_t, // return type
            ImGradient::DelegateStl, // parent class
            "get_point_count", // function name (python)
            GetPointCount // function name (c++)
        );
    }
    ImVec4 * GetPoints() override
    {
        PYBIND11_OVERRIDE_NAME(
            ImVec4 *, // return type
            ImGradient::DelegateStl, // parent class
            "get_points", // function name (python)
            GetPoints // function name (c++)
        );
    }
    std::vector<ImVec4> & GetPointsList() override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            std::vector<ImVec4> &, // return type
            ImGradient::DelegateStl, // parent class
            "get_points_list", // function name (python)
            GetPointsList // function name (c++)
        );
    }
    int EditPoint(int pointIndex, ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            int, // return type
            ImGradient::DelegateStl, // parent class
            "edit_point", // function name (python)
            EditPoint, // function name (c++)
            pointIndex, value // params
        );
    }
    ImVec4 GetPoint(float t) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            ImVec4, // return type
            ImGradient::DelegateStl, // parent class
            "get_point", // function name (python)
            GetPoint, // function name (c++)
            t // params
        );
    }
    void AddPoint(ImVec4 value) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void, // return type
            ImGradient::DelegateStl, // parent class
            "add_point", // function name (python)
            AddPoint, // function name (c++)
            value // params
        );
    }
};
}  // namespace ImGradient

// </litgen_glue_code> // Autogenerated code end
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



void py_init_module_imguizmo(nb::module_& m)
{
    using SelectedPoints = ImCurveEdit::SelectedPoints;
    using Range = ImZoomSlider::Range;
    using namespace ImZoomSlider;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // <litgen_pydef> // Autogenerated code below! Do not edit!
    ////////////////////    <generated_from:Editable.h>    ////////////////////
    auto pyClassEditable_SelectedPoints =
        py::class_<Editable<SelectedPoints>>
            (m, "Editable_SelectedPoints", " Editable: a simple structure to extend ImGui's policy of \"returning True when changed\",\n by adding with a modified return value to the functions output")
        .def(py::init<const SelectedPoints &, bool>(),
            py::arg("value"), py::arg("edited") = false)
        .def("__bool__",
            &Editable<SelectedPoints>::operator bool, "Invoke this operator to check for user modification")
        .def_readwrite("value", &Editable<SelectedPoints>::Value, "")
        .def_readwrite("edited", &Editable<SelectedPoints>::Edited, "")
        ;
    auto pyClassEditable_int =
        py::class_<Editable<int>>
            (m, "Editable_int", " Editable: a simple structure to extend ImGui's policy of \"returning True when changed\",\n by adding with a modified return value to the functions output")
        .def(py::init<const int &, bool>(),
            py::arg("value"), py::arg("edited") = false)
        .def("__bool__",
            &Editable<int>::operator bool, "Invoke this operator to check for user modification")
        .def_readwrite("value", &Editable<int>::Value, "")
        .def_readwrite("edited", &Editable<int>::Edited, "")
        ;
    auto pyClassEditable_Matrix16 =
        py::class_<Editable<Matrix16>>
            (m, "Editable_Matrix16", " Editable: a simple structure to extend ImGui's policy of \"returning True when changed\",\n by adding with a modified return value to the functions output")
        .def(py::init<const Matrix16 &, bool>(),
            py::arg("value"), py::arg("edited") = false)
        .def("__bool__",
            &Editable<Matrix16>::operator bool, "Invoke this operator to check for user modification")
        .def_readwrite("value", &Editable<Matrix16>::Value, "")
        .def_readwrite("edited", &Editable<Matrix16>::Edited, "")
        ;
    auto pyClassEditable_Range =
        py::class_<Editable<Range>>
            (m, "Editable_Range", " Editable: a simple structure to extend ImGui's policy of \"returning True when changed\",\n by adding with a modified return value to the functions output")
        .def(py::init<const Range &, bool>(),
            py::arg("value"), py::arg("edited") = false)
        .def("__bool__",
            &Editable<Range>::operator bool, "Invoke this operator to check for user modification")
        .def_readwrite("value", &Editable<Range>::Value, "")
        .def_readwrite("edited", &Editable<Range>::Edited, "")
        ;
    ////////////////////    </generated_from:Editable.h>    ////////////////////


    ////////////////////    <generated_from:ImCurveEditPure.h>    ////////////////////

    { // <namespace ImCurveEdit>
        py::module_ pyNsImCurveEdit = m.def_submodule("im_curve_edit", "");
        auto pyEnumCurveType =
            py::enum_<ImCurveEdit::CurveType>(pyNsImCurveEdit, "CurveType", py::arithmetic(), "")
                .value("curve_none", ImCurveEdit::CurveNone, "")
                .value("curve_discrete", ImCurveEdit::CurveDiscrete, "")
                .value("curve_linear", ImCurveEdit::CurveLinear, "")
                .value("curve_smooth", ImCurveEdit::CurveSmooth, "")
                .value("curve_bezier", ImCurveEdit::CurveBezier, "");


        auto pyNsImCurveEdit_ClassEditPoint =
            py::class_<ImCurveEdit::EditPoint>
                (pyNsImCurveEdit, "EditPoint", "")
            .def(py::init<>()) // implicit default constructor
            .def_readwrite("curve_index", &ImCurveEdit::EditPoint::curveIndex, "")
            .def_readwrite("point_index", &ImCurveEdit::EditPoint::pointIndex, "")
            .def("__lt__",
                &ImCurveEdit::EditPoint::operator<, py::arg("other"))
            ;


        auto pyNsImCurveEdit_ClassDelegate =
            py::class_<ImCurveEdit::Delegate, ImCurveEdit::Delegate_trampoline>
                (pyNsImCurveEdit, "Delegate", "")
            .def(py::init<>()) // implicit default constructor
            .def_readwrite("focused", &ImCurveEdit::Delegate::focused, "")
            .def("get_curve_count",
                &ImCurveEdit::Delegate::GetCurveCount)
            .def("is_visible",
                &ImCurveEdit::Delegate::IsVisible, py::arg("param_0"))
            .def("get_curve_type",
                &ImCurveEdit::Delegate::GetCurveType, py::arg("param_0"))
            .def("get_min",
                &ImCurveEdit::Delegate::GetMin)
            .def("get_max",
                &ImCurveEdit::Delegate::GetMax)
            .def("get_curve_color",
                &ImCurveEdit::Delegate::GetCurveColor, py::arg("curve_index"))
            .def("edit_point",
                &ImCurveEdit::Delegate::EditPoint, py::arg("curve_index"), py::arg("point_index"), py::arg("value"))
            .def("add_point",
                &ImCurveEdit::Delegate::AddPoint, py::arg("curve_index"), py::arg("value"))
            .def("get_background_color",
                &ImCurveEdit::Delegate::GetBackgroundColor)
            .def("begin_edit",
                &ImCurveEdit::Delegate::BeginEdit, py::arg("param_0"))
            .def("end_edit",
                &ImCurveEdit::Delegate::EndEdit)
            ;
        auto pyNsImCurveEdit_ClassDelegatePure =
            py::class_<ImCurveEdit::DelegatePure, ImCurveEdit::Delegate, ImCurveEdit::DelegatePure_trampoline>
                (pyNsImCurveEdit, "DelegatePure", "")
            .def(py::init<>()) // implicit default constructor
            .def("get_points_list",
                &ImCurveEdit::DelegatePure::GetPointsList, py::arg("curve_index"))
            ;


        pyNsImCurveEdit.def("edit_pure",
            ImCurveEdit::EditPure, py::arg("delegate"), py::arg("size"), py::arg("id"), py::arg("clipping_rect") = py::none());
    } // </namespace ImCurveEdit>
    ////////////////////    </generated_from:ImCurveEditPure.h>    ////////////////////


    ////////////////////    <generated_from:ImGradientPure.h>    ////////////////////

    { // <namespace ImGradient>
        py::module_ pyNsImGradient = m.def_submodule("im_gradient", "");
        auto pyNsImGradient_ClassDelegate =
            py::class_<ImGradient::Delegate, ImGradient::Delegate_trampoline>
                (pyNsImGradient, "Delegate", "")
            .def(py::init<>()) // implicit default constructor
            .def("edit_point",
                &ImGradient::Delegate::EditPoint, py::arg("point_index"), py::arg("value"))
            .def("get_point",
                &ImGradient::Delegate::GetPoint, py::arg("t"))
            .def("add_point",
                &ImGradient::Delegate::AddPoint, py::arg("value"))
            ;
        auto pyNsImGradient_ClassDelegateStl =
            py::class_<ImGradient::DelegateStl, ImGradient::Delegate, ImGradient::DelegateStl_trampoline>
                (pyNsImGradient, "DelegateStl", "")
            .def(py::init<>()) // implicit default constructor
            .def("get_points_list",
                &ImGradient::DelegateStl::GetPointsList)
            ;


        pyNsImGradient.def("edit_pure",
            ImGradient::EditPure, py::arg("delegate"), py::arg("size"));
    } // </namespace ImGradient>
    ////////////////////    </generated_from:ImGradientPure.h>    ////////////////////


    ////////////////////    <generated_from:ImZoomSliderPure.h>    ////////////////////

    { // <namespace ImZoomSlider>
        py::module_ pyNsImZoomSlider = m.def_submodule("im_zoom_slider", "namespace");
        auto pyEnumImGuiZoomSliderFlags_ =
            py::enum_<ImZoomSlider::ImGuiPopupFlags_>(pyNsImZoomSlider, "ImGuiZoomSliderFlags_", py::arithmetic(), "")
                .value("none", ImZoomSlider::ImGuiZoomSliderFlags_None, "")
                .value("vertical", ImZoomSlider::ImGuiZoomSliderFlags_Vertical, "")
                .value("no_anchors", ImZoomSlider::ImGuiZoomSliderFlags_NoAnchors, "")
                .value("no_middle_carets", ImZoomSlider::ImGuiZoomSliderFlags_NoMiddleCarets, "")
                .value("no_wheel", ImZoomSlider::ImGuiZoomSliderFlags_NoWheel, "");
        auto pyNsImZoomSlider_ClassRange =
            py::class_<ImZoomSlider::Range>
                (pyNsImZoomSlider, "Range", "")
            .def_readwrite("min", &ImZoomSlider::Range::Min, "")
            .def_readwrite("max", &ImZoomSlider::Range::Max, "")
            .def(py::init<float, float>(),
                py::arg("min"), py::arg("max"))
            .def("center",
                &ImZoomSlider::Range::Center)
            .def("length",
                &ImZoomSlider::Range::Length)
            ;


        pyNsImZoomSlider.def("im_zoom_slider_pure",
            ImZoomSlider::ImZoomSliderPure, py::arg("full_range"), py::arg("current_range"), py::arg("wheel_ratio") = 0.01f, py::arg("flags") = ImZoomSlider::ImGuiZoomSliderFlags_None);
    } // </namespace ImZoomSlider>
    ////////////////////    </generated_from:ImZoomSliderPure.h>    ////////////////////


    ////////////////////    <generated_from:ImGuizmoPure.h>    ////////////////////

    { // <namespace ImGuizmo>
        py::module_ pyNsImGuizmo = m.def_submodule("im_guizmo", "");
        pyNsImGuizmo.def("set_drawlist",
            ImGuizmo::SetDrawlist,
            py::arg("drawlist") = py::none(),
            " call inside your own window and before Manipulate() in order to draw gizmo to that window.\n Or pass a specific ImDrawList to draw to (e.g. ImGui::GetForegroundDrawList()).");

        pyNsImGuizmo.def("begin_frame",
            ImGuizmo::BeginFrame, "call BeginFrame right after ImGui_XXXX_NewFrame();");

        pyNsImGuizmo.def("set_im_gui_context",
            ImGuizmo::SetImGuiContext,
            py::arg("ctx"),
            " this is necessary because when imguizmo is compiled into a dll, and imgui into another\n globals are not shared between them.\n More details at https://stackoverflow.com/questions/19373061/what-happens-to-global-and-static-variables-in-a-shared-library-when-it-is-dynam\n expose method to set imgui context");

        pyNsImGuizmo.def("is_over",
            py::overload_cast<>(ImGuizmo::IsOver), "return True if mouse cursor is over any gizmo control (axis, plan or screen component)");

        pyNsImGuizmo.def("is_using",
            ImGuizmo::IsUsing, "return True if mouse IsOver or if the gizmo is in moving state");

        pyNsImGuizmo.def("is_using_any",
            ImGuizmo::IsUsingAny, "return True if any gizmo is in moving state");

        pyNsImGuizmo.def("enable",
            ImGuizmo::Enable,
            py::arg("enable"),
            " enable/disable the gizmo. Stay in the state until next call to Enable.\n gizmo is rendered with gray half transparent color when disabled");

        pyNsImGuizmo.def("set_rect",
            ImGuizmo::SetRect, py::arg("x"), py::arg("y"), py::arg("width"), py::arg("height"));

        pyNsImGuizmo.def("set_orthographic",
            ImGuizmo::SetOrthographic,
            py::arg("is_orthographic"),
            "default is False");


        auto pyEnumOPERATION =
            py::enum_<ImGuizmo::OPERATION>(pyNsImGuizmo, "OPERATION", py::arithmetic(), " call it when you want a gizmo\n Needs view and projection matrices.\n matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional\n translation is applied in world space")
                .value("translate_x", ImGuizmo::TRANSLATE_X, "")
                .value("translate_y", ImGuizmo::TRANSLATE_Y, "")
                .value("translate_z", ImGuizmo::TRANSLATE_Z, "")
                .value("rotate_x", ImGuizmo::ROTATE_X, "")
                .value("rotate_y", ImGuizmo::ROTATE_Y, "")
                .value("rotate_z", ImGuizmo::ROTATE_Z, "")
                .value("rotate_screen", ImGuizmo::ROTATE_SCREEN, "")
                .value("scale_x", ImGuizmo::SCALE_X, "")
                .value("scale_y", ImGuizmo::SCALE_Y, "")
                .value("scale_z", ImGuizmo::SCALE_Z, "")
                .value("bounds", ImGuizmo::BOUNDS, "")
                .value("scale_xu", ImGuizmo::SCALE_XU, "")
                .value("scale_yu", ImGuizmo::SCALE_YU, "")
                .value("scale_zu", ImGuizmo::SCALE_ZU, "")
                .value("translate", ImGuizmo::TRANSLATE, "")
                .value("rotate", ImGuizmo::ROTATE, "")
                .value("scale", ImGuizmo::SCALE, "")
                .value("scaleu", ImGuizmo::SCALEU, "universal")
                .value("universal", ImGuizmo::UNIVERSAL, "");


        auto pyEnumMODE =
            py::enum_<ImGuizmo::MODE>(pyNsImGuizmo, "MODE", py::arithmetic(), "")
                .value("local", ImGuizmo::LOCAL, "")
                .value("world", ImGuizmo::WORLD, "");


        pyNsImGuizmo.def("set_id",
            ImGuizmo::SetID, py::arg("id"));

        pyNsImGuizmo.def("is_over",
            py::overload_cast<ImGuizmo::OPERATION>(ImGuizmo::IsOver), py::arg("op"));

        pyNsImGuizmo.def("set_gizmo_size_clip_space",
            ImGuizmo::SetGizmoSizeClipSpace, py::arg("value"));

        pyNsImGuizmo.def("allow_axis_flip",
            ImGuizmo::AllowAxisFlip,
            py::arg("value"),
            " Allow axis to flip\n When True (default), the guizmo axis flip for better visibility\n When False, they always stay along the positive world/local axis");

        pyNsImGuizmo.def("set_axis_limit",
            ImGuizmo::SetAxisLimit,
            py::arg("value"),
            "Configure the limit where axis are hidden");

        pyNsImGuizmo.def("set_plane_limit",
            ImGuizmo::SetPlaneLimit,
            py::arg("value"),
            "Configure the limit where planes are hiden");


        auto pyEnumCOLOR =
            py::enum_<ImGuizmo::COLOR>(pyNsImGuizmo, "COLOR", py::arithmetic(), "")
                .value("direction_x", ImGuizmo::DIRECTION_X, "directionColor[0]")
                .value("direction_y", ImGuizmo::DIRECTION_Y, "directionColor[1]")
                .value("direction_z", ImGuizmo::DIRECTION_Z, "directionColor[2]")
                .value("plane_x", ImGuizmo::PLANE_X, "planeColor[0]")
                .value("plane_y", ImGuizmo::PLANE_Y, "planeColor[1]")
                .value("plane_z", ImGuizmo::PLANE_Z, "planeColor[2]")
                .value("selection", ImGuizmo::SELECTION, "selectionColor")
                .value("inactive", ImGuizmo::INACTIVE, "inactiveColor")
                .value("translation_line", ImGuizmo::TRANSLATION_LINE, "translationLineColor")
                .value("scale_line", ImGuizmo::SCALE_LINE, "")
                .value("rotation_using_border", ImGuizmo::ROTATION_USING_BORDER, "")
                .value("rotation_using_fill", ImGuizmo::ROTATION_USING_FILL, "")
                .value("hatched_axis_lines", ImGuizmo::HATCHED_AXIS_LINES, "")
                .value("text", ImGuizmo::TEXT, "")
                .value("text_shadow", ImGuizmo::TEXT_SHADOW, "")
                .value("count", ImGuizmo::COUNT, "");


        auto pyNsImGuizmo_ClassStyle =
            py::class_<ImGuizmo::Style>
                (pyNsImGuizmo, "Style", "")
            .def(py::init<>())
            .def_readwrite("translation_line_thickness", &ImGuizmo::Style::TranslationLineThickness, "Thickness of lines for translation gizmo")
            .def_readwrite("translation_line_arrow_size", &ImGuizmo::Style::TranslationLineArrowSize, "Size of arrow at the end of lines for translation gizmo")
            .def_readwrite("rotation_line_thickness", &ImGuizmo::Style::RotationLineThickness, "Thickness of lines for rotation gizmo")
            .def_readwrite("rotation_outer_line_thickness", &ImGuizmo::Style::RotationOuterLineThickness, "Thickness of line surrounding the rotation gizmo")
            .def_readwrite("scale_line_thickness", &ImGuizmo::Style::ScaleLineThickness, "Thickness of lines for scale gizmo")
            .def_readwrite("scale_line_circle_size", &ImGuizmo::Style::ScaleLineCircleSize, "Size of circle at the end of lines for scale gizmo")
            .def_readwrite("hatched_axis_line_thickness", &ImGuizmo::Style::HatchedAxisLineThickness, "Thickness of hatched axis lines")
            .def_readwrite("center_circle_size", &ImGuizmo::Style::CenterCircleSize, "Size of circle at the center of the translate/scale gizmo")
            ;


        pyNsImGuizmo.def("get_style",
            ImGuizmo::GetStyle);
        auto pyNsImGuizmo_ClassMatrixComponents =
            py::class_<ImGuizmo::MatrixComponents>
                (pyNsImGuizmo, "MatrixComponents", "")
            .def(py::init<>()) // implicit default constructor
            .def_readwrite("translation", &ImGuizmo::MatrixComponents::Translation, "")
            .def_readwrite("rotation", &ImGuizmo::MatrixComponents::Rotation, "")
            .def_readwrite("scale", &ImGuizmo::MatrixComponents::Scale, "")
            ;


        pyNsImGuizmo.def("decompose_matrix_to_components",
            py::overload_cast<const Matrix16 &>(ImGuizmo::DecomposeMatrixToComponents), py::arg("matrix"));

        pyNsImGuizmo.def("recompose_matrix_from_components",
            py::overload_cast<const ImGuizmo::MatrixComponents &>(ImGuizmo::RecomposeMatrixFromComponents), py::arg("matrix_components"));

        pyNsImGuizmo.def("draw_cubes",
            py::overload_cast<const Matrix16 &, const Matrix16 &, const std::vector<Matrix16> &>(ImGuizmo::DrawCubes), py::arg("view"), py::arg("projection"), py::arg("matrices"));

        pyNsImGuizmo.def("draw_grid",
            py::overload_cast<const Matrix16 &, const Matrix16 &, const Matrix16 &, const float>(ImGuizmo::DrawGrid), py::arg("view"), py::arg("projection"), py::arg("matrix"), py::arg("grid_size"));

        pyNsImGuizmo.def("manipulate",
            py::overload_cast<const Matrix16 &, const Matrix16 &, ImGuizmo::OPERATION, ImGuizmo::MODE, const Matrix16 &, std::optional<Matrix16>, std::optional<Matrix3>, std::optional<Matrix6>, std::optional<Matrix3>>(ImGuizmo::Manipulate),
            py::arg("view"), py::arg("projection"), py::arg("operation"), py::arg("mode"), py::arg("object_matrix"), py::arg("delta_matrix") = py::none(), py::arg("snap") = py::none(), py::arg("local_bounds") = py::none(), py::arg("bounds_snap") = py::none(),
            " Manipulate may change the objectMatrix parameter:\n if it was changed, it will return (True, newObjectMatrix)");

        pyNsImGuizmo.def("view_manipulate",
            py::overload_cast<const Matrix16 &, float, ImVec2, ImVec2, ImU32>(ImGuizmo::ViewManipulate),
            py::arg("view"), py::arg("length"), py::arg("position"), py::arg("size"), py::arg("background_color"),
            "\n Please note that this cubeview is patented by Autodesk : https://patents.google.com/patent/US7782319B2/en\n It seems to be a defensive patent in the US. I don't think it will bring troubles using it as\n other software are using the same mechanics. But just in case, you are now warned!\n\n ViewManipulate may change the view parameter: if it was changed, it will return (True, newView)");

        pyNsImGuizmo.def("view_manipulate",
            py::overload_cast<const Matrix16 &, const Matrix16 &, ImGuizmo::OPERATION, ImGuizmo::MODE, Matrix16 &, float, ImVec2, ImVec2, ImU32>(ImGuizmo::ViewManipulate),
            py::arg("view"), py::arg("projection"), py::arg("operation"), py::arg("mode"), py::arg("matrix"), py::arg("length"), py::arg("position"), py::arg("size"), py::arg("background_color"),
            " use this version if you did not call Manipulate before and you are just using ViewManipulate\n ViewManipulate may change the view parameter: if it was changed, it will return (True, newView)");
    } // </namespace ImGuizmo>
    ////////////////////    </generated_from:ImGuizmoPure.h>    ////////////////////

    // </litgen_pydef> // Autogenerated code end
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
#endif // IMGUI_BUNDLE_WITH_IMGUIZMO
