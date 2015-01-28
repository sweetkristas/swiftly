#include <map>

#include "asserts.hpp"
#include "swf_tags.hpp"

namespace swf
{
	namespace 
	{
		typedef std::map<Tag, std::string> tagname_type;

		void init_tags(tagname_type& tags) 
		{
			tags[Tag::END] = "End";
			tags[Tag::SHOW_FRAME] = "ShowFrame";
			tags[Tag::DEFINE_SHAPE] = "DefineShape";
			tags[Tag::PLACE_OBJECT] = "PlaceObject";
			tags[Tag::REMOVE_OBJECT] = "RemoveObject";
			tags[Tag::DEFINE_BITS] = "DefineBits";
			tags[Tag::DEFINE_BUTTON] = "DefineButton";
			tags[Tag::JPEG_TABLES] = "JPEGTables";
			tags[Tag::SET_BACKGROUND_COLOR_RGB] = "SetBackgroundColor";
			tags[Tag::DEFINE_FONT] = "DefineFont";
			tags[Tag::DEFINE_TEXT] = "DefineText";
			tags[Tag::DO_ACTION] = "DoAction";
			tags[Tag::DEFINE_FONT_INFO] = "DefineFontInfo";
			tags[Tag::DEFINE_SOUND] = "DefineSound";
			tags[Tag::START_SOUND] = "StartSound";
			tags[Tag::DEFINE_BUTTON_SOUND] = "DefineButtonSound";
			tags[Tag::SOUND_STREAM_HEAD] = "SoundStreamHead";
			tags[Tag::SOUND_STREAM_BLOCK] = "SoundStreamBlock";
			tags[Tag::DEFINE_BITS_LOSSLESS] = "BitsLossless";
			tags[Tag::DEFINE_BITS_JPEG2] = "DefineBitsJPEG2";
			tags[Tag::DEFINE_SHAPE2] = "DefineShape2";
			tags[Tag::DEFINE_BUTTON_CXFORM] = "DefineButtonCxform";
			tags[Tag::PROTECT] = "Protext";
			tags[Tag::PLACE_OBJECT2] = "PlaceObject2";
			tags[Tag::REMOVE_OBJECT2] = "RemoveObject2";
			tags[Tag::DEFINE_SHAPE3] = "DefineShape3";
			tags[Tag::DEFINE_TEXT2] = "DefineText2";
			tags[Tag::DEFINE_BUTTON2] = "DefineButton2";
			tags[Tag::DEFINE_BITS_JPEG3] = "DefineBitsJPEG3";
			tags[Tag::DEFINE_BITS_LOSSLESS2] = "DefineBitsLossless2";
			tags[Tag::DEFINE_EDIT_TEXT] = "DefineEditText";
			tags[Tag::DEFINE_SPRITE] = "DefineSprite";
			tags[Tag::FRAME_LABEL] = "FrameLabel";
			tags[Tag::SOUND_STREAM_HEAD2] = "SoundStreamHead2";
			tags[Tag::DEFINE_MORPH_SHAPE] = "DefineMorphShape";
			tags[Tag::DEFINE_FONT2] = "DefineFont2";
			tags[Tag::EXPORT_ASSETS] = "ExportAssets";
			tags[Tag::IMPORT_ASSETS] = "ImportAssets";
			tags[Tag::ENABLE_DEBUGGER] = "EnableDebugger";
			tags[Tag::DO_INIT_ACTION] = "DoInitAction";
			tags[Tag::DEFINE_VIDEO_STREAM] = "DefineVideoStream";
			tags[Tag::DEFINE_FONT_INFO2] = "DefineFontInfo2";
			tags[Tag::ENABLE_DEBUGGER2] = "EnableDebugger2";
			tags[Tag::SCRIPT_LIMITS] = "ScriptLimits";
			tags[Tag::SET_TAB_INDEX] = "SetTabIndex";
			tags[Tag::FILE_ATTRIBUTES] = "FileAttributes";
			tags[Tag::PLACE_OBJECT3] = "PlaceObject3";
			tags[Tag::IMPORT_ASSETS2] = "ImportAssets2";
			tags[Tag::DEFINE_FONT_ALIGN_ZONES] = "DefineFontAlignZones";
			tags[Tag::DEFINE_CSM_TEXT_SETTINGS] = "DefineCSMTextSettings";
			tags[Tag::DEFINE_FONT3] = "DefineFont3";
			tags[Tag::SYMBOL_CLASS] = "SymbolClass";
			tags[Tag::METADATA] = "Metadata";
			tags[Tag::DEFINE_SCALING_GRID] = "DefineScalingGrid";
			tags[Tag::DO_ABC] = "DoABC";
			tags[Tag::DEFINE_SHAPE4] = "DefineShape4";
			tags[Tag::DEFINE_MORPH_SHAPE2] = "DefineMorphShape2";
			tags[Tag::DEFINE_SCENE_AND_FRAME_LABEL_DATA] = "DefineSceneAndFrameLabelData";
			tags[Tag::DEFINE_BINARY_DATA] = "DefineBinaryData";
			tags[Tag::DEFINE_FONT_NAME] = "DefineFontName";
			tags[Tag::START_SOUND2] = "StartSound2";
			tags[Tag::DEFINE_JPEG_BITS4] = "DefineJPEGBits4";
			tags[Tag::DEFINE_FONT4] = "DefineFont4";
			tags[Tag::ENABLE_TELEMETRY] = "EnableTelemetry";
		}

		tagname_type& get_tag_name()
		{
			static tagname_type result;
			if(result.empty()) {
				init_tags(result);
			}
			return result;
		}
	}

	const std::string& get_tag_as_string(Tag tag)
	{
		auto it = get_tag_name().find(tag);
		ASSERT_LOG(it != get_tag_name().end(), "Unable to find name for tag value: " << static_cast<int>(tag));
		return it->second;
	}
}
