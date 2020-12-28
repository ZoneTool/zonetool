// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: aerosoul (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#pragma once

namespace ZoneTool
{
	namespace IW4
	{
		class IMenuDef : public IAsset
		{
		private:
			std::string name_;
			menuDef_t* asset_ = nullptr;

		public:
			void init(const std::string& name, ZoneMemory* mem) override;

			std::string name() override;
			std::int32_t type() override;

			static int indentCounter;
			static FILE* fp;
			static void dump(menuDef_t* asset);

			static void emit_menu_def(menuDef_t* asset);
			static void emit_item_def(itemDef_t* item);
			static void emit_window_def(windowDef_t* window, bool is_item);
			static void emit_set_local_var_data(SetLocalVarData* data, EventType type);
			static void emit_conditional_script(ConditionalScript* script);
			static void emit_statement(const char* name, Statement_s* statement, bool semiColon = false);
			static void emit_menu_event_handler_set(const char* name, MenuEventHandlerSet* set);
			static void emit_item_key_handler(const char* name, ItemKeyHandler* handler);
			static void emit_item_float_expressions(ItemFloatExpression* expression, int count);
			static void emit_column_info(columnInfo_s* columns, int count);
			static void emit_list_box(listBoxDef_s* listBox);
			static void emit_multi_def(multiDef_s* multiDef);
			static void emit_item_def_data(itemDefData_t* data, int type);
			static void emit_dvar_flags(int dvarFlags, const char* dvarTest, const char* enableDvar);
			static void emit_static_flags(int flags);
			static void emit_dynamic_flags(int flags);
			static void emit_color(const char* name, vec4_t& color);
			static void emit_rect(const char* name, rectDef_s& rect);
			static void emit_string(const char* name, const char* value);
			static void emit_float(const char* name, float value);
			static void emit_int(const char* name, int value);
			static void emit_bool(const char* name, bool value);

			static void emit_open_brace();
			static void emit_closing_brace();

			static void push_indent();
			static void pop_indent();
			static const char* get_tabs();
			static void replace_all(std::string& output, std::string from, std::string to, bool case_insensitive = false);
			static std::string escape_string(const char* value);
			static std::string format_script(const char* value);
		};
	}
}