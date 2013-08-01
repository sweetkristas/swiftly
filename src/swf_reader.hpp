#pragma once

#include <string>
#include <zlib.h>
#include "bit_reader.hpp"
#include "swf.hpp"

namespace swf
{
	class reader
	{
	public:
		explicit reader(const std::string& fname, swf& obj);
		enum tagName {
			TAG_END,
			TAG_SHOW_FRAME,
			TAG_DEFINE_SHAPE,
			TAG_PLACE_OBJECT = 4,
			TAG_REMOVE_OBJECT,
			TAG_DEFINE_BITS,
			TAG_DEFINE_BUTTON,
			TAG_JPEG_TABLES,
			TAG_SET_BACKGROUND_COLOR,
			TAG_DEFINE_FONT,
			TAG_DEFINE_TEXT,
			TAG_DO_ACTION,
			TAG_DEFINE_FONT_INFO,
			TAG_DEFINE_SOUND,
			TAG_START_SOUND,
			TAG_DEFINE_BUTTON_SOUND = 17,
			TAG_SOUND_STREAM_HEAD,
			TAG_SOUND_STREAM_BLOCK,
			TAG_DEFINE_BITS_LOSSLESS,
			TAG_DEFINE_BITS_JPEG2,
			TAG_DEFINE_SHAPE2,
			TAG_DEFINE_BUTTON_CXFORM,
			TAG_PROTECT,
			TAG_PLACE_OBJECT2 = 26,
			TAG_REMOVE_OBJECT2 = 28,
			TAG_DEFINE_SHAPE3 = 32,
			TAG_DEFINE_TEXT2,
			TAG_DEFINE_BUTTON2,
			TAG_DEFINE_BITS_JPEG3,
			TAG_DEFINE_BITS_LOSSLESS2,
			TAG_DEFINE_EDIT_TEXT,
			TAG_DEFINE_SPRITE = 39,
			TAG_FRAME_LABEL = 43,
			TAG_SOUND_STREAM_HEAD2 = 45,
			TAG_DEFINE_MORPH_SHAPE,
			TAG_DEFINE_FONT2 = 48,
			TAG_EXPORT_ASSETS = 56,
			TAG_IMPORT_ASSETS,
			TAG_ENABLE_DEBUGGER,
			TAG_DO_INIT_ACTION,
			TAG_DEFINE_VIDEO_STREAM,
			TAG_DEFINE_FONT_INFO2,
			TAG_ENABLE_DEBUGGER2 = 64,
			TAG_SCRIPT_LIMITS,
			TAG_SET_TAB_INDEX,
			TAG_FILE_ATTRIBUTES = 69,
			TAG_PLACE_OBJECT3,
			TAG_IMPORT_ASSETS2,
			TAG_DEFINE_FONT_ALIGN_ZONES = 73,
			TAG_DEFINE_CSM_TEXT_SETTINGS,
			TAG_DEFINE_FONT3,
			TAG_SYMBOL_CLASS,
			TAG_METADATA,
			TAG_DEFINE_SCALING_GRID,
			TAG_DO_ABC = 82,
			TAG_DEFINE_SHAPE4,
			TAG_DEFINE_MORPH_SHAPE2,
			TAG_DEFINE_SCENE_AND_FRAME_LABEL_DATA = 86,
			TAG_DEFINE_BINARY_DATA,
			TAG_DEFINE_FONT_NAME,
			TAG_START_SOUND2,
			TAG_DEFINE_JPEG_BITS4,
			TAG_DEFINE_FONT4,
			TAG_ENABLE_TELEMETRY,
		};
	private:
		std::shared_ptr<bit_stream> bits_;
		swf& obj_;

		void ProcessShowFrame(unsigned length);
		void ProcessDefineShape(unsigned length);
		void ProcessPlaceObject(unsigned length);
		void ProcessRemoveObject(unsigned length);
		void ProcessDefineBits(unsigned length);
		void ProcessDefineButton(unsigned length);
		void ProcessJPEGTables(unsigned length);
		void ProcessSetBackgroundColor(unsigned length);
		void ProcessDefineFont(unsigned length);
		void ProcessDefineText(unsigned length);
		void ProcessDoAction(unsigned length);
		void ProcessDefineFontInfo(unsigned length);
		void ProcessDefineSound(unsigned length);
		void ProcessStartSound(unsigned length);
		void ProcessDefineButtonSound(unsigned length);
		void ProcessSoundStreamHead(unsigned length);
		void ProcessSoundStreamBlock(unsigned length);
		void ProcessBitsLossless(unsigned length);
		void ProcessDefineBitsJPEG2(unsigned length);
		void ProcessDefineShape2(unsigned length);
		void ProcessDefineButtonCxform(unsigned length);
		void ProcessProtect(unsigned length);
		void ProcessPlaceObject2(unsigned length);
		void ProcessRemoveObject2(unsigned length);
		void ProcessDefineShape3(unsigned length);
		void ProcessDefineText2(unsigned length);
		void ProcessDefineButton2(unsigned length);
		void ProcessDefineBitsJPEG3(unsigned length);
		void ProcessDefineBitsLossless2(unsigned length);
		void ProcessDefineEditText(unsigned length);
		void ProcessDefineSprite(unsigned length);
		void ProcessFrameLabel(unsigned length);
		void ProcessSoundStreamHead2(unsigned length);
		void ProcessDefineMorphShape(unsigned length);
		void ProcessDefineFont2(unsigned length);
		void ProcessExportAssets(unsigned length);
		void ProcessImportAssets(unsigned length);
		void ProcessEnableDebugger(unsigned length);
		void ProcessDoInitAction(unsigned length);
		void ProcessDefineVideoStream(unsigned length);
		void ProcessDefineFontInfo2(unsigned length);
		void ProcessEnableDebugger2(unsigned length);
		void ProcessScriptLimits(unsigned length);
		void ProcessSetTabIndex(unsigned length);
		void ProcessFileAttributes(unsigned length);
		void ProcessPlaceObject3(unsigned length);
		void ProcessImportAssets2(unsigned length);
		void ProcessDefineFontAlignZones(unsigned length);
		void ProcessDefineCSMTextSettings(unsigned length);
		void ProcessDefineFont3(unsigned length);
		void ProcessSymbolClass(unsigned length);
		void ProcessMetadata(unsigned length);
		void ProcessDefineScalingGrid(unsigned length);
		void ProcessDoABC(unsigned length);
		void ProcessDefineShape4(unsigned length);
		void ProcessDefineMorphShape2(unsigned length);
		void ProcessDefineSceneAndFrameLabelData(unsigned length);
		void ProcessDefineBinaryData(unsigned length);
		void ProcessDefineFontName(unsigned length);
		void ProcessStartSound2(unsigned length);
		void ProcessDefineJPEGBits4(unsigned length);
		void ProcessDefineFont4(unsigned length);
		void ProcessEnableTelemetry(unsigned length);

		void read_tags();
		void eat_bit_stream(unsigned length);

		reader();
		reader(const reader&);
	};
}
