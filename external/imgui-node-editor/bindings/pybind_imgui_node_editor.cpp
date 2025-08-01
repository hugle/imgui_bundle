// Part of ImGui Bundle - MIT License - Copyright (c) 2022-2024 Pascal Thomet - https://github.com/pthom/imgui_bundle
#ifdef IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/function.h>
#include <nanobind/ndarray.h>

#include "imgui-node-editor/imgui_node_editor_internal.h"
#include "imgui-node-editor/imgui_node_editor.h"
#include "imgui_node_editor_immapp/node_editor_default_context.h"


namespace nb = nanobind;


namespace ax
{
    namespace NodeEditor
    {
        // using EditorContext = Detail::EditorContext;
        struct EditorContext: public Detail::EditorContext {};
    }
}


namespace
{
    uintptr_t _static_next_editor_id = 1;

    uintptr_t get_next_id()
    {
        uintptr_t r = _static_next_editor_id;
        ++ _static_next_editor_id;
        return r;
    }
}


extern std::function<void()> FnResetImGuiNodeEditorId;


NB_MAKE_OPAQUE(ax::NodeEditor::Detail::EditorContext);
NB_MAKE_OPAQUE(ax::NodeEditor::EditorContext);


// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// <litgen_glue_code>  // Autogenerated code below! Do not edit!

// </litgen_glue_code> // Autogenerated code end
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


void py_init_module_imgui_node_editor(nb::module_& m)
{
    using namespace ax::NodeEditor;
    using CanvasSizeModeAlias = ax::NodeEditor::CanvasSizeMode;

    FnResetImGuiNodeEditorId = []() {
        _static_next_editor_id = 1;
    };

    nb::class_<EditorContext>(m, "EditorContext");

    nb::class_<NodeId>(m, "NodeId")
        .def(nb::init<>())
        .def(nb::init<uintptr_t>())
        .def("create", []() { return NodeId(get_next_id()); })
        .def("id", [](const NodeId& self) { return self.Get(); })
        .def("__eq__", [](const NodeId& self, const NodeId& other) {
            return self.Get() == other.Get();
        })
        .def("__str__", [](const NodeId& self) { return std::to_string(self.Get()); })
        .def("__repr__", [](const NodeId& self) { return std::to_string(self.Get()); })
        ;
    nb::class_<LinkId>(m, "LinkId")
        .def(nb::init<>())
        .def(nb::init<uintptr_t>())
        .def("create", []() { return LinkId(get_next_id()); })
        .def("id", [](const LinkId& self) { return self.Get(); })
        .def("__eq__", [](const LinkId& self, const LinkId& other) {
            return self.Get() == other.Get();
        })
        .def("__str__", [](const LinkId& self) { return std::to_string(self.Get()); })
        .def("__repr__", [](const LinkId& self) { return std::to_string(self.Get()); })
        ;
    nb::class_<PinId>(m, "PinId")
        .def(nb::init<>())
        .def(nb::init<uintptr_t>())
        .def("create", []() { return PinId(get_next_id()); })
        .def("id", [](const PinId& self) { return self.Get(); })
        .def("__eq__", [](const PinId& self, const PinId& other) {
            return self.Get() == other.Get();
        })
        .def("__str__", [](const PinId& self) { return std::to_string(self.Get()); })
        .def("__repr__", [](const PinId& self) { return std::to_string(self.Get()); })
        ;

    m.def("suspend_editor_canvas", []() {
       GetCurrentEditor()->Suspend();
    });
    m.def("resume_editor_canvas", []() {
        GetCurrentEditor()->Resume();
    });

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // <litgen_pydef> // Autogenerated code below! Do not edit!
    ////////////////////    <generated_from:imgui_node_editor.h>    ////////////////////
    // # ifndef __IMGUI_NODE_EDITOR_H__
    //
    // #ifdef IMGUI_BUNDLE_PYTHON_API
    //
    // #endif
    //


    auto pyEnumPinKind =
        nb::enum_<ax::NodeEditor::PinKind>(m, "PinKind", nb::is_arithmetic(), "------------------------------------------------------------------------------")
            .value("input", ax::NodeEditor::PinKind::Input, "")
            .value("output", ax::NodeEditor::PinKind::Output, "");


    auto pyEnumFlowDirection =
        nb::enum_<ax::NodeEditor::FlowDirection>(m, "FlowDirection", nb::is_arithmetic(), "")
            .value("forward", ax::NodeEditor::FlowDirection::Forward, "")
            .value("backward", ax::NodeEditor::FlowDirection::Backward, "");


    auto pyEnumCanvasSizeMode =
        nb::enum_<ax::NodeEditor::CanvasSizeMode>(m, "CanvasSizeMode", nb::is_arithmetic(), "")
            .value("fit_vertical_view", ax::NodeEditor::CanvasSizeMode::FitVerticalView, "Previous view will be scaled to fit new view on Y axis")
            .value("fit_horizontal_view", ax::NodeEditor::CanvasSizeMode::FitHorizontalView, "Previous view will be scaled to fit new view on X axis")
            .value("center_only", ax::NodeEditor::CanvasSizeMode::CenterOnly, "Previous view will be centered on new view");


    auto pyEnumSaveReasonFlags =
        nb::enum_<ax::NodeEditor::SaveReasonFlags>(m, "SaveReasonFlags", nb::is_arithmetic(), "------------------------------------------------------------------------------")
            .value("none", ax::NodeEditor::SaveReasonFlags::None, "")
            .value("navigation", ax::NodeEditor::SaveReasonFlags::Navigation, "")
            .value("position", ax::NodeEditor::SaveReasonFlags::Position, "")
            .value("size", ax::NodeEditor::SaveReasonFlags::Size, "")
            .value("selection", ax::NodeEditor::SaveReasonFlags::Selection, "")
            .value("add_node", ax::NodeEditor::SaveReasonFlags::AddNode, "")
            .value("remove_node", ax::NodeEditor::SaveReasonFlags::RemoveNode, "")
            .value("user", ax::NodeEditor::SaveReasonFlags::User, "");


    auto pyClassConfig =
        nb::class_<ax::NodeEditor::Config>
            (m, "Config", "")
        .def_rw("settings_file", &ax::NodeEditor::Config::SettingsFile, "")
        .def_rw("user_pointer", &ax::NodeEditor::Config::UserPointer, "")
        .def_rw("canvas_size_mode", &ax::NodeEditor::Config::CanvasSizeMode, "")
        .def_rw("drag_button_index", &ax::NodeEditor::Config::DragButtonIndex, "Mouse button index drag action will react to (0-left, 1-right, 2-middle)")
        .def_rw("select_button_index", &ax::NodeEditor::Config::SelectButtonIndex, "Mouse button index select action will react to (0-left, 1-right, 2-middle)")
        .def_rw("navigate_button_index", &ax::NodeEditor::Config::NavigateButtonIndex, "Mouse button index navigate action will react to (0-left, 1-right, 2-middle)")
        .def_rw("context_menu_button_index", &ax::NodeEditor::Config::ContextMenuButtonIndex, "Mouse button index context menu action will react to (0-left, 1-right, 2-middle)")
        .def_rw("enable_smooth_zoom", &ax::NodeEditor::Config::EnableSmoothZoom, "")
        .def_rw("smooth_zoom_power", &ax::NodeEditor::Config::SmoothZoomPower, "")
        .def_rw("force_window_content_width_to_node_width", &ax::NodeEditor::Config::ForceWindowContentWidthToNodeWidth, " [ADAPT_IMGUI_BUNDLE]\n\n By default, ImGui::TextWrapped() and ImGui::Separator(), and ImGui::SliderXXX\n will not work in a Node because they will not respect the node's bounds.\n Instead, they will use the width of the whole window.\n Set ForceWindowContentWidthToNodeWidth to True to fix this (this is disabled by default).")
        .def(nb::init<>())
        ;


    auto pyEnumStyleColor =
        nb::enum_<ax::NodeEditor::StyleColor>(m, "StyleColor", nb::is_arithmetic(), "------------------------------------------------------------------------------")
            .value("bg", ax::NodeEditor::StyleColor_Bg, "")
            .value("grid", ax::NodeEditor::StyleColor_Grid, "")
            .value("node_bg", ax::NodeEditor::StyleColor_NodeBg, "")
            .value("node_border", ax::NodeEditor::StyleColor_NodeBorder, "")
            .value("hov_node_border", ax::NodeEditor::StyleColor_HovNodeBorder, "")
            .value("sel_node_border", ax::NodeEditor::StyleColor_SelNodeBorder, "")
            .value("node_sel_rect", ax::NodeEditor::StyleColor_NodeSelRect, "")
            .value("node_sel_rect_border", ax::NodeEditor::StyleColor_NodeSelRectBorder, "")
            .value("hov_link_border", ax::NodeEditor::StyleColor_HovLinkBorder, "")
            .value("sel_link_border", ax::NodeEditor::StyleColor_SelLinkBorder, "")
            .value("highlight_link_border", ax::NodeEditor::StyleColor_HighlightLinkBorder, "")
            .value("link_sel_rect", ax::NodeEditor::StyleColor_LinkSelRect, "")
            .value("link_sel_rect_border", ax::NodeEditor::StyleColor_LinkSelRectBorder, "")
            .value("pin_rect", ax::NodeEditor::StyleColor_PinRect, "")
            .value("pin_rect_border", ax::NodeEditor::StyleColor_PinRectBorder, "")
            .value("flow", ax::NodeEditor::StyleColor_Flow, "")
            .value("flow_marker", ax::NodeEditor::StyleColor_FlowMarker, "")
            .value("group_bg", ax::NodeEditor::StyleColor_GroupBg, "")
            .value("group_border", ax::NodeEditor::StyleColor_GroupBorder, "")
            .value("count", ax::NodeEditor::StyleColor_Count, "");


    auto pyEnumStyleVar =
        nb::enum_<ax::NodeEditor::StyleVar>(m, "StyleVar", nb::is_arithmetic(), "")
            .value("node_padding", ax::NodeEditor::StyleVar_NodePadding, "")
            .value("node_rounding", ax::NodeEditor::StyleVar_NodeRounding, "")
            .value("node_border_width", ax::NodeEditor::StyleVar_NodeBorderWidth, "")
            .value("hovered_node_border_width", ax::NodeEditor::StyleVar_HoveredNodeBorderWidth, "")
            .value("selected_node_border_width", ax::NodeEditor::StyleVar_SelectedNodeBorderWidth, "")
            .value("pin_rounding", ax::NodeEditor::StyleVar_PinRounding, "")
            .value("pin_border_width", ax::NodeEditor::StyleVar_PinBorderWidth, "")
            .value("link_strength", ax::NodeEditor::StyleVar_LinkStrength, "")
            .value("source_direction", ax::NodeEditor::StyleVar_SourceDirection, "")
            .value("target_direction", ax::NodeEditor::StyleVar_TargetDirection, "")
            .value("scroll_duration", ax::NodeEditor::StyleVar_ScrollDuration, "")
            .value("flow_marker_distance", ax::NodeEditor::StyleVar_FlowMarkerDistance, "")
            .value("flow_speed", ax::NodeEditor::StyleVar_FlowSpeed, "")
            .value("flow_duration", ax::NodeEditor::StyleVar_FlowDuration, "")
            .value("pivot_alignment", ax::NodeEditor::StyleVar_PivotAlignment, "")
            .value("pivot_size", ax::NodeEditor::StyleVar_PivotSize, "")
            .value("pivot_scale", ax::NodeEditor::StyleVar_PivotScale, "")
            .value("pin_corners", ax::NodeEditor::StyleVar_PinCorners, "")
            .value("pin_radius", ax::NodeEditor::StyleVar_PinRadius, "")
            .value("pin_arrow_size", ax::NodeEditor::StyleVar_PinArrowSize, "")
            .value("pin_arrow_width", ax::NodeEditor::StyleVar_PinArrowWidth, "")
            .value("group_rounding", ax::NodeEditor::StyleVar_GroupRounding, "")
            .value("group_border_width", ax::NodeEditor::StyleVar_GroupBorderWidth, "")
            .value("highlight_connected_links", ax::NodeEditor::StyleVar_HighlightConnectedLinks, "")
            .value("snap_link_to_pin_dir", ax::NodeEditor::StyleVar_SnapLinkToPinDir, "")
            .value("hovered_node_border_offset", ax::NodeEditor::StyleVar_HoveredNodeBorderOffset, "")
            .value("selected_node_border_offset", ax::NodeEditor::StyleVar_SelectedNodeBorderOffset, "")
            .value("count", ax::NodeEditor::StyleVar_Count, "");


    auto pyClassStyle =
        nb::class_<ax::NodeEditor::Style>
            (m, "Style", "")
        .def_rw("node_padding", &ax::NodeEditor::Style::NodePadding, "")
        .def_rw("node_rounding", &ax::NodeEditor::Style::NodeRounding, "")
        .def_rw("node_border_width", &ax::NodeEditor::Style::NodeBorderWidth, "")
        .def_rw("hovered_node_border_width", &ax::NodeEditor::Style::HoveredNodeBorderWidth, "")
        .def_rw("hover_node_border_offset", &ax::NodeEditor::Style::HoverNodeBorderOffset, "")
        .def_rw("selected_node_border_width", &ax::NodeEditor::Style::SelectedNodeBorderWidth, "")
        .def_rw("selected_node_border_offset", &ax::NodeEditor::Style::SelectedNodeBorderOffset, "")
        .def_rw("pin_rounding", &ax::NodeEditor::Style::PinRounding, "")
        .def_rw("pin_border_width", &ax::NodeEditor::Style::PinBorderWidth, "")
        .def_rw("link_strength", &ax::NodeEditor::Style::LinkStrength, "")
        .def_rw("source_direction", &ax::NodeEditor::Style::SourceDirection, "")
        .def_rw("target_direction", &ax::NodeEditor::Style::TargetDirection, "")
        .def_rw("scroll_duration", &ax::NodeEditor::Style::ScrollDuration, "")
        .def_rw("flow_marker_distance", &ax::NodeEditor::Style::FlowMarkerDistance, "")
        .def_rw("flow_speed", &ax::NodeEditor::Style::FlowSpeed, "")
        .def_rw("flow_duration", &ax::NodeEditor::Style::FlowDuration, "")
        .def_rw("pivot_alignment", &ax::NodeEditor::Style::PivotAlignment, "")
        .def_rw("pivot_size", &ax::NodeEditor::Style::PivotSize, "")
        .def_rw("pivot_scale", &ax::NodeEditor::Style::PivotScale, "")
        .def_rw("pin_corners", &ax::NodeEditor::Style::PinCorners, "")
        .def_rw("pin_radius", &ax::NodeEditor::Style::PinRadius, "")
        .def_rw("pin_arrow_size", &ax::NodeEditor::Style::PinArrowSize, "")
        .def_rw("pin_arrow_width", &ax::NodeEditor::Style::PinArrowWidth, "")
        .def_rw("group_rounding", &ax::NodeEditor::Style::GroupRounding, "")
        .def_rw("group_border_width", &ax::NodeEditor::Style::GroupBorderWidth, "")
        .def_rw("highlight_connected_links", &ax::NodeEditor::Style::HighlightConnectedLinks, "")
        .def_rw("snap_link_to_pin_dir", &ax::NodeEditor::Style::SnapLinkToPinDir, "when True link will start on the line defined by pin direction")
        // #ifdef IMGUI_BUNDLE_PYTHON_API
        //
        .def("color_",
            &ax::NodeEditor::Style::Color_,
            nb::arg("idx_color"),
            nb::rv_policy::reference)
        .def("set_color_",
            &ax::NodeEditor::Style::SetColor_, nb::arg("idx_color"), nb::arg("color"))
        // #endif
        //
        .def(nb::init<>())
        ;


    m.def("set_current_editor",
        ax::NodeEditor::SetCurrentEditor, nb::arg("ctx"));

    m.def("get_current_editor",
        ax::NodeEditor::GetCurrentEditor, nb::rv_policy::reference);

    m.def("create_editor",
        ax::NodeEditor::CreateEditor,
        nb::arg("config") = nb::none(),
        nb::rv_policy::reference);

    m.def("destroy_editor",
        ax::NodeEditor::DestroyEditor, nb::arg("ctx"));

    m.def("get_config",
        ax::NodeEditor::GetConfig,
        nb::arg("ctx") = nb::none(),
        nb::rv_policy::reference);

    m.def("get_style",
        ax::NodeEditor::GetStyle, nb::rv_policy::reference);

    m.def("get_style_color_name",
        ax::NodeEditor::GetStyleColorName,
        nb::arg("color_index"),
        nb::rv_policy::reference);

    m.def("push_style_color",
        ax::NodeEditor::PushStyleColor, nb::arg("color_index"), nb::arg("color"));

    m.def("pop_style_color",
        ax::NodeEditor::PopStyleColor, nb::arg("count") = 1);

    m.def("push_style_var",
        nb::overload_cast<ax::NodeEditor::StyleVar, float>(ax::NodeEditor::PushStyleVar), nb::arg("var_index"), nb::arg("value"));

    m.def("push_style_var",
        nb::overload_cast<ax::NodeEditor::StyleVar, const ImVec2 &>(ax::NodeEditor::PushStyleVar), nb::arg("var_index"), nb::arg("value"));

    m.def("push_style_var",
        nb::overload_cast<ax::NodeEditor::StyleVar, const ImVec4 &>(ax::NodeEditor::PushStyleVar), nb::arg("var_index"), nb::arg("value"));

    m.def("pop_style_var",
        ax::NodeEditor::PopStyleVar, nb::arg("count") = 1);

    m.def("begin",
        [](const char * id, const std::optional<const ImVec2> & size = std::nullopt)
        {
            auto Begin_adapt_mutable_param_with_default_value = [](const char * id, const std::optional<const ImVec2> & size = std::nullopt)
            {

                const ImVec2& size_or_default = [&]() -> const ImVec2 {
                    if (size.has_value())
                        return size.value();
                    else
                        return ImVec2(0, 0);
                }();

                ax::NodeEditor::Begin(id, size_or_default);
            };

            Begin_adapt_mutable_param_with_default_value(id, size);
        },
        nb::arg("id"), nb::arg("size").none() = nb::none(),
        "Python bindings defaults:\n    If size is None, then its default value will be: ImVec2(0, 0)");

    m.def("end",
        ax::NodeEditor::End);

    m.def("begin_node",
        ax::NodeEditor::BeginNode, nb::arg("id"));

    m.def("begin_pin",
        ax::NodeEditor::BeginPin, nb::arg("id"), nb::arg("kind"));

    m.def("pin_rect",
        ax::NodeEditor::PinRect, nb::arg("a"), nb::arg("b"));

    m.def("pin_pivot_rect",
        ax::NodeEditor::PinPivotRect, nb::arg("a"), nb::arg("b"));

    m.def("pin_pivot_size",
        ax::NodeEditor::PinPivotSize, nb::arg("size"));

    m.def("pin_pivot_scale",
        ax::NodeEditor::PinPivotScale, nb::arg("scale"));

    m.def("pin_pivot_alignment",
        ax::NodeEditor::PinPivotAlignment, nb::arg("alignment"));

    m.def("end_pin",
        ax::NodeEditor::EndPin);

    m.def("group",
        ax::NodeEditor::Group, nb::arg("size"));

    m.def("end_node",
        ax::NodeEditor::EndNode);

    m.def("begin_group_hint",
        ax::NodeEditor::BeginGroupHint, nb::arg("node_id"));

    m.def("get_group_min",
        ax::NodeEditor::GetGroupMin);

    m.def("get_group_max",
        ax::NodeEditor::GetGroupMax);

    m.def("get_hint_foreground_draw_list",
        ax::NodeEditor::GetHintForegroundDrawList, nb::rv_policy::reference);

    m.def("get_hint_background_draw_list",
        ax::NodeEditor::GetHintBackgroundDrawList, nb::rv_policy::reference);

    m.def("end_group_hint",
        ax::NodeEditor::EndGroupHint);

    m.def("get_node_background_draw_list",
        ax::NodeEditor::GetNodeBackgroundDrawList,
        nb::arg("node_id"),
        "TODO: Add a way to manage node background channels",
        nb::rv_policy::reference);

    m.def("link",
        [](ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId, const std::optional<const ImVec4> & color = std::nullopt, float thickness = 1.0f) -> bool
        {
            auto Link_adapt_mutable_param_with_default_value = [](ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId, const std::optional<const ImVec4> & color = std::nullopt, float thickness = 1.0f) -> bool
            {

                const ImVec4& color_or_default = [&]() -> const ImVec4 {
                    if (color.has_value())
                        return color.value();
                    else
                        return ImVec4(1, 1, 1, 1);
                }();

                auto lambda_result = ax::NodeEditor::Link(id, startPinId, endPinId, color_or_default, thickness);
                return lambda_result;
            };

            return Link_adapt_mutable_param_with_default_value(id, startPinId, endPinId, color, thickness);
        },
        nb::arg("id"), nb::arg("start_pin_id"), nb::arg("end_pin_id"), nb::arg("color").none() = nb::none(), nb::arg("thickness") = 1.0f,
        "Python bindings defaults:\n    If color is None, then its default value will be: ImVec4(1, 1, 1, 1)");

    m.def("flow",
        ax::NodeEditor::Flow, nb::arg("link_id"), nb::arg("direction") = ax::NodeEditor::FlowDirection::Forward);

    m.def("begin_create",
        [](const std::optional<const ImVec4> & color = std::nullopt, float thickness = 1.0f) -> bool
        {
            auto BeginCreate_adapt_mutable_param_with_default_value = [](const std::optional<const ImVec4> & color = std::nullopt, float thickness = 1.0f) -> bool
            {

                const ImVec4& color_or_default = [&]() -> const ImVec4 {
                    if (color.has_value())
                        return color.value();
                    else
                        return ImVec4(1, 1, 1, 1);
                }();

                auto lambda_result = ax::NodeEditor::BeginCreate(color_or_default, thickness);
                return lambda_result;
            };

            return BeginCreate_adapt_mutable_param_with_default_value(color, thickness);
        },
        nb::arg("color").none() = nb::none(), nb::arg("thickness") = 1.0f,
        "Python bindings defaults:\n    If color is None, then its default value will be: ImVec4(1, 1, 1, 1)");

    m.def("query_new_link",
        nb::overload_cast<ax::NodeEditor::PinId *, ax::NodeEditor::PinId *>(ax::NodeEditor::QueryNewLink), nb::arg("start_id"), nb::arg("end_id"));

    m.def("query_new_link",
        nb::overload_cast<ax::NodeEditor::PinId *, ax::NodeEditor::PinId *, const ImVec4 &, float>(ax::NodeEditor::QueryNewLink), nb::arg("start_id"), nb::arg("end_id"), nb::arg("color"), nb::arg("thickness") = 1.0f);

    m.def("query_new_node",
        nb::overload_cast<ax::NodeEditor::PinId *>(ax::NodeEditor::QueryNewNode), nb::arg("pin_id"));

    m.def("query_new_node",
        nb::overload_cast<ax::NodeEditor::PinId *, const ImVec4 &, float>(ax::NodeEditor::QueryNewNode), nb::arg("pin_id"), nb::arg("color"), nb::arg("thickness") = 1.0f);

    m.def("accept_new_item",
        nb::overload_cast<>(ax::NodeEditor::AcceptNewItem));

    m.def("accept_new_item",
        nb::overload_cast<const ImVec4 &, float>(ax::NodeEditor::AcceptNewItem), nb::arg("color"), nb::arg("thickness") = 1.0f);

    m.def("reject_new_item",
        nb::overload_cast<>(ax::NodeEditor::RejectNewItem));

    m.def("reject_new_item",
        nb::overload_cast<const ImVec4 &, float>(ax::NodeEditor::RejectNewItem), nb::arg("color"), nb::arg("thickness") = 1.0f);

    m.def("end_create",
        ax::NodeEditor::EndCreate);

    m.def("begin_delete",
        ax::NodeEditor::BeginDelete);

    m.def("query_deleted_link",
        ax::NodeEditor::QueryDeletedLink, nb::arg("link_id"), nb::arg("start_id") = nb::none(), nb::arg("end_id") = nb::none());

    m.def("query_deleted_node",
        ax::NodeEditor::QueryDeletedNode, nb::arg("node_id"));

    m.def("accept_deleted_item",
        ax::NodeEditor::AcceptDeletedItem, nb::arg("delete_dependencies") = true);

    m.def("reject_deleted_item",
        ax::NodeEditor::RejectDeletedItem);

    m.def("end_delete",
        ax::NodeEditor::EndDelete);

    m.def("set_node_position",
        ax::NodeEditor::SetNodePosition, nb::arg("node_id"), nb::arg("editor_position"));

    m.def("set_group_size",
        ax::NodeEditor::SetGroupSize, nb::arg("node_id"), nb::arg("size"));

    m.def("get_node_position",
        ax::NodeEditor::GetNodePosition, nb::arg("node_id"));

    m.def("get_node_size",
        ax::NodeEditor::GetNodeSize, nb::arg("node_id"));

    m.def("center_node_on_screen",
        ax::NodeEditor::CenterNodeOnScreen, nb::arg("node_id"));

    m.def("set_node_z_position",
        ax::NodeEditor::SetNodeZPosition,
        nb::arg("node_id"), nb::arg("z"),
        "Sets node z position, nodes with higher value are drawn over nodes with lower value");

    m.def("get_node_z_position",
        ax::NodeEditor::GetNodeZPosition,
        nb::arg("node_id"),
        "Returns node z position, defaults is 0.0");

    m.def("restore_node_state",
        ax::NodeEditor::RestoreNodeState, nb::arg("node_id"));

    m.def("suspend",
        ax::NodeEditor::Suspend);

    m.def("resume",
        ax::NodeEditor::Resume);

    m.def("is_suspended",
        ax::NodeEditor::IsSuspended);

    m.def("is_active",
        ax::NodeEditor::IsActive);

    m.def("has_selection_changed",
        ax::NodeEditor::HasSelectionChanged);

    m.def("get_selected_object_count",
        ax::NodeEditor::GetSelectedObjectCount);
    // #ifdef IMGUI_BUNDLE_PYTHON_API
    //

    m.def("get_selected_nodes",
        []() -> std::vector<ax::NodeEditor::NodeId>
        {
            auto GetSelectedNodes_adapt_force_lambda = []() -> std::vector<ax::NodeEditor::NodeId>
            {
                auto lambda_result = ax::NodeEditor::GetSelectedNodes();
                return lambda_result;
            };

            return GetSelectedNodes_adapt_force_lambda();
        });

    m.def("get_selected_links",
        []() -> std::vector<ax::NodeEditor::LinkId>
        {
            auto GetSelectedLinks_adapt_force_lambda = []() -> std::vector<ax::NodeEditor::LinkId>
            {
                auto lambda_result = ax::NodeEditor::GetSelectedLinks();
                return lambda_result;
            };

            return GetSelectedLinks_adapt_force_lambda();
        });
    // #endif
    //

    m.def("is_node_selected",
        ax::NodeEditor::IsNodeSelected, nb::arg("node_id"));

    m.def("is_link_selected",
        ax::NodeEditor::IsLinkSelected, nb::arg("link_id"));

    m.def("clear_selection",
        ax::NodeEditor::ClearSelection);

    m.def("select_node",
        ax::NodeEditor::SelectNode, nb::arg("node_id"), nb::arg("append") = false);

    m.def("select_link",
        ax::NodeEditor::SelectLink, nb::arg("link_id"), nb::arg("append") = false);

    m.def("deselect_node",
        ax::NodeEditor::DeselectNode, nb::arg("node_id"));

    m.def("deselect_link",
        ax::NodeEditor::DeselectLink, nb::arg("link_id"));

    m.def("delete_node",
        ax::NodeEditor::DeleteNode, nb::arg("node_id"));

    m.def("delete_link",
        ax::NodeEditor::DeleteLink, nb::arg("link_id"));

    m.def("has_any_links",
        nb::overload_cast<ax::NodeEditor::NodeId>(ax::NodeEditor::HasAnyLinks),
        nb::arg("node_id"),
        "Returns True if node has any link connected");

    m.def("has_any_links",
        nb::overload_cast<ax::NodeEditor::PinId>(ax::NodeEditor::HasAnyLinks),
        nb::arg("pin_id"),
        "Return True if pin has any link connected");

    m.def("break_links",
        nb::overload_cast<ax::NodeEditor::NodeId>(ax::NodeEditor::BreakLinks),
        nb::arg("node_id"),
        "Break all links connected to this node");

    m.def("break_links",
        nb::overload_cast<ax::NodeEditor::PinId>(ax::NodeEditor::BreakLinks),
        nb::arg("pin_id"),
        "Break all links connected to this pin");

    m.def("navigate_to_content",
        ax::NodeEditor::NavigateToContent, nb::arg("duration") = -1);

    m.def("navigate_to_selection",
        ax::NodeEditor::NavigateToSelection, nb::arg("zoom_in") = false, nb::arg("duration") = -1);

    m.def("show_node_context_menu",
        ax::NodeEditor::ShowNodeContextMenu, nb::arg("node_id"));

    m.def("show_pin_context_menu",
        ax::NodeEditor::ShowPinContextMenu, nb::arg("pin_id"));

    m.def("show_link_context_menu",
        ax::NodeEditor::ShowLinkContextMenu, nb::arg("link_id"));

    m.def("show_background_context_menu",
        ax::NodeEditor::ShowBackgroundContextMenu);

    m.def("enable_shortcuts",
        ax::NodeEditor::EnableShortcuts, nb::arg("enable"));

    m.def("are_shortcuts_enabled",
        ax::NodeEditor::AreShortcutsEnabled);

    m.def("begin_shortcut",
        ax::NodeEditor::BeginShortcut);

    m.def("accept_cut",
        ax::NodeEditor::AcceptCut);

    m.def("accept_copy",
        ax::NodeEditor::AcceptCopy);

    m.def("accept_paste",
        ax::NodeEditor::AcceptPaste);

    m.def("accept_duplicate",
        ax::NodeEditor::AcceptDuplicate);

    m.def("accept_create_node",
        ax::NodeEditor::AcceptCreateNode);

    m.def("get_action_context_size",
        ax::NodeEditor::GetActionContextSize);
    // #ifdef IMGUI_BUNDLE_PYTHON_API
    //

    m.def("get_action_context_nodes",
        []() -> std::vector<ax::NodeEditor::NodeId>
        {
            auto GetActionContextNodes_adapt_force_lambda = []() -> std::vector<ax::NodeEditor::NodeId>
            {
                auto lambda_result = ax::NodeEditor::GetActionContextNodes();
                return lambda_result;
            };

            return GetActionContextNodes_adapt_force_lambda();
        });

    m.def("get_action_context_links",
        []() -> std::vector<ax::NodeEditor::LinkId>
        {
            auto GetActionContextLinks_adapt_force_lambda = []() -> std::vector<ax::NodeEditor::LinkId>
            {
                auto lambda_result = ax::NodeEditor::GetActionContextLinks();
                return lambda_result;
            };

            return GetActionContextLinks_adapt_force_lambda();
        });
    // #endif
    //

    m.def("end_shortcut",
        ax::NodeEditor::EndShortcut);

    m.def("get_current_zoom",
        ax::NodeEditor::GetCurrentZoom);

    m.def("get_hovered_node",
        ax::NodeEditor::GetHoveredNode);

    m.def("get_hovered_pin",
        ax::NodeEditor::GetHoveredPin);

    m.def("get_hovered_link",
        ax::NodeEditor::GetHoveredLink);

    m.def("get_double_clicked_node",
        ax::NodeEditor::GetDoubleClickedNode);

    m.def("get_double_clicked_pin",
        ax::NodeEditor::GetDoubleClickedPin);

    m.def("get_double_clicked_link",
        ax::NodeEditor::GetDoubleClickedLink);

    m.def("is_background_clicked",
        ax::NodeEditor::IsBackgroundClicked);

    m.def("is_background_double_clicked",
        ax::NodeEditor::IsBackgroundDoubleClicked);

    m.def("get_background_click_button_index",
        ax::NodeEditor::GetBackgroundClickButtonIndex, "-1 if none");

    m.def("get_background_double_click_button_index",
        ax::NodeEditor::GetBackgroundDoubleClickButtonIndex, "-1 if none");

    m.def("get_link_pins",
        ax::NodeEditor::GetLinkPins,
        nb::arg("link_id"), nb::arg("start_pin_id"), nb::arg("end_pin_id"),
        "pass None if particular pin do not interest you");

    m.def("pin_had_any_links",
        ax::NodeEditor::PinHadAnyLinks, nb::arg("pin_id"));

    m.def("get_screen_size",
        ax::NodeEditor::GetScreenSize);

    m.def("screen_to_canvas",
        ax::NodeEditor::ScreenToCanvas, nb::arg("pos"));

    m.def("canvas_to_screen",
        ax::NodeEditor::CanvasToScreen, nb::arg("pos"));

    m.def("get_node_count",
        ax::NodeEditor::GetNodeCount, "Returns number of submitted nodes since Begin() call");
    // #ifdef IMGUI_BUNDLE_PYTHON_API
    //

    m.def("get_ordered_node_ids",
        []() -> std::vector<ax::NodeEditor::NodeId>
        {
            auto GetOrderedNodeIds_adapt_force_lambda = []() -> std::vector<ax::NodeEditor::NodeId>
            {
                auto lambda_result = ax::NodeEditor::GetOrderedNodeIds();
                return lambda_result;
            };

            return GetOrderedNodeIds_adapt_force_lambda();
        },     "Fills an array with node id's in order they're drawn");
    // #endif
    //
    // # endif
    ////////////////////    </generated_from:imgui_node_editor.h>    ////////////////////


    ////////////////////    <generated_from:node_editor_default_context.h>    ////////////////////
    m.def("default_node_editor_context_immapp",
        DefaultNodeEditorContext_Immapp, nb::rv_policy::reference);

    m.def("suspend_node_editor_canvas_immapp",
        SuspendNodeEditorCanvas_Immapp);

    m.def("resume_node_editor_canvas_immapp",
        ResumeNodeEditorCanvas_Immapp);

    m.def("disable_user_input_this_frame",
        DisableUserInputThisFrame);

    m.def("update_node_editor_colors_from_imgui_colors",
        UpdateNodeEditorColorsFromImguiColors);
    ////////////////////    </generated_from:node_editor_default_context.h>    ////////////////////

    // </litgen_pydef> // Autogenerated code end
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  AUTOGENERATED CODE END !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
#endif // IMGUI_BUNDLE_WITH_IMGUI_NODE_EDITOR
