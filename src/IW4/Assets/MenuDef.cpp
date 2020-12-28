// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: aerosoul (https://github.com/aerosoul94)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		void IMenuDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).menu;
		}

		std::string IMenuDef::name()
		{
			return this->name_;
		}

		std::int32_t IMenuDef::type()
		{
			return menu;
		}

		int IMenuDef::indentCounter = 0;
		FILE* IMenuDef::fp = nullptr;

		static const char* g_expOperatorNames[]
		{
			"NOOP",
			")",
			"*",
			"/",
			"%",
			"+",
			"-",
			"!",
			"<",
			"<=",
			">",
			">=",
			"==",
			"!=",
			"&&",
			"||",
			"(",
			",",
			"&",
			"|",
			"~",
			"<<",
			">>",
			"dvarint(static)",
			"dvarbool(static)",
			"dvarfloat(static)",
			"dvarstring(static)",
			"int",
			"string",
			"float",
			"sin",
			"cos",
			"min",
			"max",
			"milliseconds",
			"dvarint",
			"dvarbool",
			"dvarfloat",
			"dvarstring",
			"stat",
			"ui_active",
			"flashbanged",
			"usingvehicle",
			"missilecam",
			"scoped",
			"scopedthermal",
			"scoreboard_visible",
			"inkillcam",
			"inkillcamnpc",
			"player",
			"getperk",
			"selecting_location",
			"selecting_direction",
			"team",
			"otherteam",
			"marinesfield",
			"opforfield",
			"menuisopen",
			"writingdata",
			"inlobby",
			"inprivateparty",
			"privatepartyhost",
			"privatepartyhostinlobby",
			"aloneinparty",
			"adsjavelin",
			"weaplockblink",
			"weapattacktop",
			"weapattackdirect",
			"weaplocking",
			"weaplocked",
			"weaplocktooclose",
			"weaplockscreenposx",
			"weaplockscreenposy",
			"secondsastime",
			"tablelookup",
			"tablelookupbyrow",
			"tablegetrownum",
			"locstring",
			"localvarint",
			"localvarbool",
			"localvarfloat",
			"localvarstring",
			"timeleft",
			"secondsascountdown",
			"gamemsgwndactive",
			"gametypename",
			"gametype",
			"gametypedescription",
			"scoreatrank",
			"friendsonline",
			"spectatingclient",
			"spectatingfree",
			"statrangeanybitsset",
			"keybinding",
			"actionslotusable",
			"hudfade",
			"maxrecommendedplayers",
			"acceptinginvite",
			"isintermission",
			"gamehost",
			"partyismissingmappack",
			"partymissingmappackerror",
			"anynewmappacks",
			"amiselected",
			"partystatusstring",
			"attachedcontrollercount",
			"issplitscreenonlinepossible",
			"splitscreenplayercount",
			"getplayerdata",
			"getplayerdatasplitscreen",
			"experienceforlevel",
			"levelforexperience",
			"isitemunlocked",
			"isitemunlockedsplitscreen",
			"debugprint",
			"getplayerdataanybooltrue",
			"weaponclassnew",
			"weaponname",
			"isreloading",
			"savegameavailable",
			"unlockeditemcount",
			"unlockeditemcountsplitscreen",
			"unlockeditem",
			"unlockeditemsplitscreen",
			"mailsubject",
			"mailfrom",
			"mailreceived",
			"mailbody",
			"maillootlocalized",
			"mailgivesloot",
			"anynewmail",
			"mailtimetofollowup",
			"mailloottype",
			"mailranlottery",
			"lotterylootlocalized",
			"radarisjammed",
			"radarjamintensity",
			"radarisenabled",
			"isempjammed",
			"playerads",
			"weaponheatactive",
			"weaponheatvalue",
			"weaponheatoverheated",
			"getsplashtext",
			"getsplashdescription",
			"getsplashmaterial",
			"splashhasicon",
			"splashrownum",
			"getfocuseditemname",
			"getfocuseditemx",
			"getfocuseditemy",
			"getfocuseditemwidth",
			"getfocuseditemheight",
			"getitemx",
			"getitemy",
			"getitemwidth",
			"getitemheight",
			"playlist",
			"scoreboardexternalmutenotice",
			"getclientmatchdata",
			"getclientmatchdatadef",
			"getmapname",
			"getmapimage",
			"getmapcustom",
			"getmigrationstatus",
			"getplayercardinfo",
			"isofflineprofileselected",
			"coopplayer",
			"iscoop",
			"getpartystatus",
			"getsearchparams",
			"gettimeplayed",
			"isselectedplayerfriend",
			"getcharbyindex",
			"getprofiledata",
			"isprofilesignedin",
			"getwaitpopupstatus",
			"getnattype",
			"getlocalizednattype",
			"getadjustedsafeareahorizontal",
			"getadjustedsafeareavertical",
			"connectioninfo",
			"offlineprofilecansave",
			"allsplitscreenprofilescansave",
			"allsplitscreenprofilesaresignedin",
			"coopready"
		};

		const char* g_commandList[] =
		{
			"fadein",
			"fadeout",
			"show",
			"hide",
			"showMenu",
			"hideMenu",
			"setcolor",
			"open",
			"close",
			"closeForAllPlayers",
			"ingameopen",
			"ingameclose",
			"setbackground",
			"setItemColor",
			"focusfirst",
			"setfocus",
			"setfocusbydvar",
			"setdvar",
			"exec",
			"execnow",
			"execOnDvarStringValue",
			"execOnDvarIntValue",
			"execOnDvarFloatValue",
			"execNowOnDvarStringValue",
			"execNowOnDvarIntValue",
			"execNowOnDvarFloatValue",
			"play",
			"scriptmenuresponse",
			"scriptMenuRespondOnDvarStringValue",
			"scriptMenuRespondOnDvarIntValue",
			"scriptMenuRespondOnDvarFloatValue",
			"setPlayerData",
			"setPlayerDataSplitscreen",
			"updateMail",
			"openMail",
			"deleteMail",
			"doMailLottery",
			"resetStatsConfirm",
			"resetStatsCancel",
			"setGameMode",
			"setLocalVarBool",
			"setLocalVarInt",
			"setLocalVarFloat",
			"setLocalVarString",
			"feederTop",
			"feederBottom",
			"showGamerCard",
			"openforgametype",
			"closeforgametype",
			"statclearperknew",
			"statsetusingtable",
			"statclearbitmask",
			"kickPlayer",
			"getKickPlayerQuestion",
			"partyUpdateMissingMapPackDvar",
			"getHostMigrateQuestion",
			"makehost",
			"togglePlayerMute",
			"addFriendFromId",
			"resolveError",
			"lerp",
			// added by me
			"uiScript"
		};

		struct ItemFloatExpressionEntry
		{
			int target;
			const char* s1;
			const char* s2;
		};

		ItemFloatExpressionEntry g_itemFloatExpressions[19] =
		{
			{ 0, "rect", "x" },
			{ 1, "rect", "y" },
			{ 2, "rect", "w" },
			{ 3, "rect", "h" },
			{ 4, "forecolor", "r" },
			{ 5, "forecolor", "g" },
			{ 6, "forecolor", "b" },
			{ 7, "forecolor", "rgb" },
			{ 8, "forecolor", "a" },
			{ 9, "glowcolor", "r" },
			{ 10, "glowcolor", "g" },
			{ 11, "glowcolor", "b" },
			{ 12, "glowcolor", "rgb" },
			{ 13, "glowcolor", "a" },
			{ 14, "backcolor", "r" },
			{ 15, "backcolor", "g" },
			{ 16, "backcolor", "b" },
			{ 17, "backcolor", "rgb" },
			{ 18, "backcolor", "a" }
		};

		void IMenuDef::dump(menuDef_t* asset)
		{
			ZONETOOL_INFO("Dumping menu %s", asset->window.name);

			fp = FileSystem::FileOpen("menus\\"s + asset->window.name + ".menu"s, "w");
			indentCounter = 1;

			if (fp)
			{
				emit_menu_def(asset);
			}

			FileSystem::FileClose(fp);
		}

		void IMenuDef::emit_open_brace()
		{
			fprintf(fp, "%s{\n", get_tabs());
		}

		void IMenuDef::emit_closing_brace()
		{
			fprintf(fp, "%s}\n", get_tabs());
		}

		void IMenuDef::push_indent()
		{
			emit_open_brace();
			IMenuDef::indentCounter++;
		}

		void IMenuDef::pop_indent()
		{
			IMenuDef::indentCounter--;
			emit_closing_brace();
		}

		void IMenuDef::emit_menu_def(menuDef_t* asset)
		{
			fputs("{\n\tmenuDef\n", fp);
			push_indent();

			emit_window_def(&asset->window, false);
			emit_color("focuscolor", asset->focusColor);
			emit_int("fullscreen", asset->fullscreen);
			emit_float("fadeCycle", asset->fadeCycle);
			emit_float("fadeClamp", asset->fadeClamp);
			emit_float("fadeAmount", asset->fadeAmount);
			emit_float("fadeInAmount", asset->fadeInAmount);
			emit_float("blurWorld", asset->blurRadius);
			emit_string("allowedBinding", asset->allowedBinding);
			emit_statement("visible when", asset->visibleExp);
			emit_statement("exp rect x", asset->rectXExp);
			emit_statement("exp rect y", asset->rectYExp);
			emit_statement("exp rect h", asset->rectHExp);
			emit_statement("exp rect w", asset->rectWExp);
			emit_statement("exp openSound", asset->openSoundExp);
			emit_statement("exp closeSound", asset->closeSoundExp);
			emit_item_key_handler("execKeyInt", asset->onKey);
			emit_menu_event_handler_set("onOpen", asset->onOpen);
			emit_menu_event_handler_set("onRequestClose", asset->onRequestClose);
			emit_menu_event_handler_set("onClose", asset->onClose);
			emit_menu_event_handler_set("onEsc", asset->onEsc);

			for (auto i = 0; i < asset->itemCount; i++)
			{
				emit_item_def(asset->items[i]);
			}

			pop_indent();
			fputs("}\n", fp);
		}

		void IMenuDef::emit_item_def(itemDef_t* item)
		{
			fputs("\t\titemDef\n", fp);
			push_indent();

			emit_window_def(&item->window, true);
			emit_int("type", item->type);
			emit_int("align", item->alignment);
			emit_int("textfont", item->fontEnum);
			emit_int("textalign", item->textAlignMode);
			emit_float("textalignx", item->textAlignX);
			emit_float("textaligny", item->textAlignY);
			emit_float("textscale", item->textScale);
			emit_color("glowColor", item->glowColor);
			emit_bool("decodeEffect", item->decayActive);
			
			if (item->type == ITEM_TYPE_GAME_MESSAGE_WINDOW)
			{
				emit_int("gamemsgwindowindex", item->gameMsgWindowIndex);
				emit_int("gamemsgwindowmode", item->gameMsgWindowMode);
			}

			emit_string("text", item->text);
			emit_bool("textsavegame", item->textSaveGameInfo & 1);
			emit_bool("textcinematicsubtitle", item->textSaveGameInfo & 2);
			emit_float("feeder", item->special);
			emit_string("focusSound", item->focusSound);

			if (item->type != ITEM_TYPE_SLIDER && item->type != ITEM_TYPE_DVARENUM)
			{
				emit_string("dvar", item->dvar);
				emit_item_def_data(&item->typeData, item->type);
			}
			else
			{
				emit_item_def_data(&item->typeData, item->type);
			}

			emit_dvar_flags(item->dvarFlags, item->dvarTest, item->enableDvar);
			emit_item_float_expressions(item->floatExpressions, item->floatExpressionCount);
			emit_statement("visible when", item->visibleExp);
			emit_statement("disabled when", item->disabledExp);
			emit_statement("exp text", item->textExp);
			emit_statement("exp material", item->materialExp);
			emit_item_key_handler("execKeyInt", item->onKey);
			emit_menu_event_handler_set("mouseEnterText", item->mouseEnterText);
			emit_menu_event_handler_set("mouseExitText", item->mouseExitText);
			emit_menu_event_handler_set("mouseEnter", item->mouseEnter);
			emit_menu_event_handler_set("mouseExit", item->mouseExit);
			emit_menu_event_handler_set("action", item->action);
			emit_menu_event_handler_set("accept", item->accept);
			emit_menu_event_handler_set("onFocus", item->onFocus);
			emit_menu_event_handler_set("leaveFocus", item->leaveFocus);

			pop_indent();
		}

		void IMenuDef::emit_window_def(windowDef_t* window, bool is_item)
		{
			emit_string("name", window->name);
			if (is_item)
			{
				emit_rect("rect", window->rectClient);
			}
			else
			{
				emit_rect("rect", window->rect);
			}
			emit_static_flags(window->staticFlags);
			emit_dynamic_flags(window->dynamicFlags);
			emit_string("group", window->group);
			emit_int("style", window->style);
			emit_int("border", window->border);
			emit_float("borderSize", window->borderSize);
			emit_int("ownerdraw", window->ownerDraw);
			emit_int("ownerdrawFlag", window->ownerDrawFlags);
			emit_color("forecolor", window->foreColor);
			emit_color("backcolor", window->backColor);
			emit_color("bordercolor", window->borderColor);
			emit_color("outlinecolor", window->outlineColor);
			emit_color("disablecolor", window->disableColor);

			if (window->background)
			{
				emit_string("background", window->background->name);
			}
		}

		void IMenuDef::emit_set_local_var_data(SetLocalVarData* data, EventType type)
		{
			if (data)
			{
				std::string cmd;
				switch (type)
				{
				case EVENT_SET_LOCAL_VAR_BOOL: 
					cmd = "setLocalVarBool"; 
					break;
				case EVENT_SET_LOCAL_VAR_INT: 
					cmd = "setLocalVarInt"; 
					break;
				case EVENT_SET_LOCAL_VAR_FLOAT: 
					cmd = "setLocalVarFloat"; 
					break;
				case EVENT_SET_LOCAL_VAR_STRING: 
					cmd = "setLocalVarString"; 
					break;
				}

				fprintf(fp, "%s%s \"%s\"", get_tabs(), cmd.c_str(), data->localVarName);
				emit_statement(nullptr, data->expression, true);
			}
		}

		void IMenuDef::emit_dynamic_flags(int flags)
		{
			emit_bool("visible 1", flags & WINDOWDYNAMIC_VISIBLE);
		}

		void IMenuDef::emit_static_flags(int flags)
		{
			emit_bool("decoration", flags & WINDOWSTATIC_DECORATION);
			emit_bool("horizontalscroll", flags & WINDOWSTATIC_HORIZONTALSCROLL);
			emit_bool("screenSpace", flags & WINDOWSTATIC_SCREENSPACE);
			emit_bool("autowrapped", flags & WINDOWSTATIC_AUTOWRAPPED);
			emit_bool("popup", flags & WINDOWSTATIC_POPUP);
			emit_bool("outOfBoundsClick", flags & WINDOWSTATIC_OUTOFBOUNDSCLICK);
			emit_bool("legacySplitScreenScale", flags & WINDOWSTATIC_LEGACYSPLITSCREENSCALE);
			emit_bool("hiddenDuringFlashbang", flags & WINDOWSTATIC_HIDDENDURINGFLASH);
			emit_bool("hiddenDuringScope", flags & WINDOWSTATIC_HIDDENDURINGSCOPE);
			emit_bool("hiddenDuringUI", flags & WINDOWSTATIC_HIDDENDURINGUI);
			emit_bool("textOnlyFocus", flags & WINDOWSTATIC_TEXTONLYFOCUS);
		}

		void IMenuDef::emit_dvar_flags(int dvarFlags, const char* dvarTest, const char* enableDvar)
		{
			std::string command;
			switch (dvarFlags)
			{
			case 0x01: 
				command = "enableDvar"; 
				break;
			case 0x02: 
				command = "disableDvar"; 
				break;
			case 0x04: 
				command = "showDvar"; 
				break;
			case 0x08: 
				command = "hideDvar"; 
				break;
			case 0x10: 
				command = "focusDvar"; 
				break;
			}

			emit_string("dvarTest", dvarTest);

			if (enableDvar)
			{
				fprintf(fp, "%s%s { %s}\n", get_tabs(), command.c_str(), enableDvar);
			}
		}

		void IMenuDef::emit_item_def_data(itemDefData_t* data, int type)
		{
			if (data->data)
			{
				switch (type)
				{
				case ITEM_TYPE_LISTBOX:
					emit_list_box(data->listBox);
					break;
				case ITEM_TYPE_MULTI:
					emit_multi_def(data->multiDef);
					break;
				case ITEM_TYPE_DVARENUM:
					fprintf(fp, "%s%s %s %s", get_tabs(), "dvar", "dvarEnumList", data->enumDvarName);
					break;
				case ITEM_TYPE_NEWSTICKER:
					emit_int("newsfeed", data->ticker->feedId);
					emit_int("speed", data->ticker->speed);
					emit_int("spacing", data->ticker->spacing);
					break;
				case ITEM_TYPE_EDITFIELD:
				case ITEM_TYPE_NUMERICFIELD:
				case ITEM_TYPE_VALIDFILEFIELD:
				case ITEM_TYPE_UPREDITFIELD:
				case ITEM_TYPE_YESNO:
				case ITEM_TYPE_BIND:
				case ITEM_TYPE_TEXT:
				case ITEM_TYPE_DECIMALFIELD:
				case ITEM_TYPE_EMAILFIELD:
				case ITEM_TYPE_PASSWORDFIELD:
					emit_int("maxChars", data->editField->maxChars);
					emit_int("maxCharsGotoNext", data->editField->maxCharsGotoNext);
					emit_int("maxPaintChars", data->editField->maxPaintChars);
					break;
				case ITEM_TYPE_TEXTSCROLL:
				default:
					break;
				}
			}
		}

		void IMenuDef::emit_multi_def(multiDef_s* multiDef)
		{
			if (multiDef->strDef)
			{
				fprintf(fp, "%s%s {", get_tabs(), "dvarStrList");
			}
			else
			{
				fprintf(fp, "%s%s {", get_tabs(), "dvarFloatList");
			}

			for (auto i = 0; i < multiDef->count; i++)
			{
				fprintf(fp, " \"%s\"", multiDef->dvarList[i]);
				if (multiDef->strDef)
				{
					fprintf(fp, " \"%s\"", multiDef->dvarStr[i]);
				}
				else
				{
					fprintf(fp, " %g", multiDef->dvarValue[i]);
				}
			}

			fputs(" }\n", fp);
		}

		void IMenuDef::emit_list_box(listBoxDef_s* listBox)
		{
			emit_color("selectBorder", listBox->selectBorder);

			if (listBox->selectIcon)
			{
				emit_string("selectIcon", listBox->selectIcon->name);
			}

			emit_float("elementWidth", listBox->elementWidth);
			emit_float("elementHeight", listBox->elementHeight);
			emit_int("elementtype", listBox->elementStyle);
			emit_column_info(listBox->columnInfo, listBox->numColumns);
			emit_bool("noscrollbars", listBox->noscrollbars);
			emit_bool("notselectable", listBox->notselectable);
			emit_bool("usepaging", listBox->usepaging);
			emit_menu_event_handler_set("doubleClick", listBox->doubleClick);
		}

		void IMenuDef::emit_column_info(columnInfo_s* columns, int count)
		{
			fprintf(fp, "//%snumcol\t\t\txpos\txwidth\ttextlen\t  alignment\n", get_tabs());
			fprintf(fp, "%s%s %i\t\t%i\t\t%i\t\t%i\t\t  %i\n", get_tabs(), "columns", count, columns[0].xpos, columns[0].width, columns[0].maxChars, columns[0].alignment);
			for (int i = 1; i < count; i++)
			{
				fprintf(fp, "%s\t\t\t\t%i\t\t%i\t\t%i\t\t  %i\n", get_tabs(), columns[i].xpos, columns[i].width, columns[i].maxChars, columns[i].alignment);
			}
		}

		void IMenuDef::emit_item_float_expressions(ItemFloatExpression* expressions, int count)
		{
			if (expressions)
			{
				for (auto i = 0; i < count; i++)
				{
					std::string name;
					name = "exp ";
					name += g_itemFloatExpressions[expressions[i].target].s1;
					name += " ";
					name += g_itemFloatExpressions[expressions[i].target].s2;
					emit_statement(name.c_str(), expressions[i].expression);
				}
			}
		}

		void IMenuDef::emit_item_key_handler(const char* name, ItemKeyHandler* handler)
		{
			if (handler)
			{
				emit_menu_event_handler_set("", handler->action);
				emit_item_key_handler("execKeyInt", handler->next);
			}
		}

		void IMenuDef::emit_menu_event_handler_set(const char* name, MenuEventHandlerSet* set)
		{
			if (set)
			{
				const bool formatStatement = true;

				if (name)
				{
					fprintf(fp, "%s%s\n", get_tabs(), name);
					push_indent();
				}

				for (auto i = 0; i < set->eventHandlerCount; i++)
				{
					switch (set->eventHandlers[i]->eventType)
					{
					case EVENT_UNCONDITIONAL:
						fprintf(fp, "%s%s", get_tabs(), format_script(set->eventHandlers[i]->eventData.unconditionalScript).c_str());
						break;
					case EVENT_IF:
						emit_conditional_script(set->eventHandlers[i]->eventData.conditionalScript);
						break;
					case EVENT_ELSE:
						emit_menu_event_handler_set("else", set->eventHandlers[i]->eventData.elseScript);
						break;
					case EVENT_SET_LOCAL_VAR_BOOL:
					case EVENT_SET_LOCAL_VAR_INT:
					case EVENT_SET_LOCAL_VAR_FLOAT:
					case EVENT_SET_LOCAL_VAR_STRING:
						emit_set_local_var_data(set->eventHandlers[i]->eventData.setLocalVarData, set->eventHandlers[i]->eventType);
						break;
					default:
						break;
					}
				}

				if (name)
				{
					pop_indent();
				}
			}
		}

		void IMenuDef::emit_conditional_script(ConditionalScript* script)
		{
			if (script)
			{
				fprintf(fp, "%sif", get_tabs());
				emit_statement(nullptr, script->eventExpression);
				push_indent();
				emit_menu_event_handler_set(nullptr, script->eventHandlerSet);
				pop_indent();
			}
		}

		void IMenuDef::emit_statement(const char* name, Statement_s* statement, bool semiColon)
		{
			if (statement)
			{
				const bool clean_statements = true;
				bool needs_closing_parenthesis = false;

				if (name)
				{
					fprintf(fp, "%s%s", get_tabs(), name);
				}

				if (clean_statements)
				{
					if (statement->entries[0].data.op != OP_LEFTPAREN)
					{
						fputs(" (", fp);
					}
				}

				for (auto i = 0; i < statement->numEntries; i++)
				{
					int type = statement->entries[i].type;
					if (type == OPERATOR)
					{
						needs_closing_parenthesis = true;
						auto op = statement->entries[i].data.op;
						if (op < OP_FIRSTFUNCTIONCALL)
						{
							if (statement->entries[i - 1].data.op == OP_RIGHTPAREN && op == OP_LEFTPAREN)
							{
								++i;
								continue;
							}

							if (statement->entries[i - 1].data.op == OP_NOT)
							{
								fprintf(fp, "%s", g_expOperatorNames[op]);
							}
							else
							{
								fprintf(fp, " %s", g_expOperatorNames[op]);
							}
						}
						else if (op >= OP_STATICDVARINT && op <= OP_STATICDVARSTRING)
						{
							i++;

							std::string command;
							switch (op)
							{
							case OP_STATICDVARINT:
								command = "dvarstring";
								break;
							case OP_STATICDVARBOOL:
								command = "dvarbool";
								break;
							case OP_STATICDVARFLOAT:
								command = "dvarfloat";
								break;
							case OP_STATICDVARSTRING:
								command = "dvarstring";
								break;
							}

							command += "( \"";
							command += statement->supportingData->staticDvarList.staticDvars[statement->entries[i].data.operand.internals.intVal]->dvarName;
							command += "\" )";

							if (statement->entries[i - 2].data.op == OP_NOT)
							{
								fprintf(fp, "%s", command.c_str());
							}
							else
							{
								fprintf(fp, " %s", command.c_str());
							}

							i++;
						}
						else
						{
							if (statement->entries[i - 1].data.op == OP_NOT)
								fprintf(fp, "%s(", g_expOperatorNames[op]);
							else
								fprintf(fp, " %s(", g_expOperatorNames[op]);
						}
					}
					else if (type == OPERAND)
					{
						switch (statement->entries[i].data.operand.dataType)
						{
						case VAL_INT:
							fprintf(fp, " %i", statement->entries[i].data.operand.internals.intVal);
							break;
						case VAL_FLOAT:
							fprintf(fp, " %g", statement->entries[i].data.operand.internals.floatVal);
							break;
						case VAL_STRING:
							fprintf(fp, " \"%s\"", escape_string(statement->entries[i].data.operand.internals.stringVal.string).c_str());
							break;
						case VAL_FUNCTION:
							emit_statement(nullptr, statement->entries[i].data.operand.internals.function);
							break;
						}
					}
				}

				if (clean_statements)
				{
					if (needs_closing_parenthesis)
					{
						fputs(" )", fp);
					}

					if (statement->entries[0].data.op != OP_LEFTPAREN)
					{
						fputs(" )", fp);
					}

					if (semiColon)
					{
						fputs(";", fp);
					}
				}

				fputs("\n", fp);
			}
		}

		void IMenuDef::emit_string(const char* name, const char* value)
		{
			if (value && *value)
			{
				fprintf(fp, "%s%s \"%s\"\n", get_tabs(), name, value);
			}
		}

		void IMenuDef::emit_bool(const char* name, bool value)
		{
			if (value)
			{
				fprintf(fp, "%s%s\n", get_tabs(), name);
			}
		}

		void IMenuDef::emit_int(const char* name, int value)
		{
			if (value)
			{
				fprintf(fp, "%s%s %i\n", get_tabs(), name, value);
			}
		}

		void IMenuDef::emit_float(const char* name, float value)
		{
			if (value)
			{
				fprintf(fp, "%s%s %g\n", get_tabs(), name, value);
			}
		}

		void IMenuDef::emit_rect(const char* name, rectDef_s& rect)
		{
			// TODO: Some pre-processing may be needed
			fprintf(fp, "%s%s %g %g %g %g %i %i\n", get_tabs(), name, rect.x, rect.y, rect.w, rect.h, rect.horzAlign, rect.vertAlign);
		}

		void IMenuDef::emit_color(const char* name, vec4_t& color)
		{
			if (color[0] > 0.0f || color[1] > 0.0f || color[2] > 0.0f || color[3] > 0.0f)
			{
				fprintf(fp, "%s%s %g %g %g %g\n", get_tabs(), name, color[0], color[1], color[2], color[3]);
			}
		}

		const char* IMenuDef::get_tabs()
		{
			static char tabs[10];

			for (auto i = 0; i < indentCounter && i < 10; i++)
			{
				tabs[i] = '\t';
			}

			tabs[indentCounter] = 0;
			return tabs;
		}

		void IMenuDef::replace_all(std::string& str, std::string from, std::string to, bool case_insensitive)
		{
			if (case_insensitive)
			{
				auto replace = [&](std::string from, std::string to) -> bool
				{
					std::string lowered(str);
					std::transform(str.begin(), str.end(), lowered.begin(), ::tolower);

					auto pos = lowered.find(from);
					if (pos == std::string::npos)
					{
						return false;
					}

					str.replace(pos, from.length(), to);
					return true;
				};

				std::transform(from.begin(), from.end(), from.begin(), ::tolower);
				while (replace(from, to));
			}
			else
			{
				std::size_t pos = 0;
				while ((pos = str.find(from, pos)) != std::string::npos)
				{
					str.replace(pos, from.length(), to);
					pos += to.length();
				}
			}
		}

		std::string IMenuDef::escape_string(const char* value)
		{
			std::string out(value);
			replace_all(out, "\t", "\\t");
			replace_all(out, "\n", "\\n");
			return out;
		}

		std::string IMenuDef::format_script(const char* value)
		{
			std::string out = escape_string(value);
			const std::string tabs = get_tabs();

			// clean indentation and semi colons
			out += "\n";
			replace_all(out, "\t;", "\t");
			replace_all(out, tabs + "\n", "");
			replace_all(out, " ; \n", ";\n");
			replace_all(out, " \n", ";\n");
			replace_all(out, " ; ", ";\n" + tabs);
			replace_all(out, "; ", ";\n" + tabs);

			// remove quotes from keywords
			replace_all(out, "\"self\"", "self");
			replace_all(out, "\"forecolor\"", "forecolor");
			replace_all(out, "\"backcolor\"", "backcolor");
			replace_all(out, "\"bordercolor\"", "bordercolor");

			// remove quotes from commands
			for (auto i = 0; i < 62; i++)
			{
				replace_all(out, "\""s + g_commandList[i] + "\""s, g_commandList[i], true);
			}

			return out;
		}
	}
}