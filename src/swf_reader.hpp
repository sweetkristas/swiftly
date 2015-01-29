#pragma once

#include <string>
#include <zlib.h>

#include "bit_reader.hpp"
#include "swf_fwd.hpp"

namespace swf
{
	class reader
	{
	public:
		explicit reader(const std::string& fname, const player_ptr& play);
	private:
		player_ptr player_;
		std::shared_ptr<bit_stream> bits_;

		void ProcessShowFrame(const character_def_ptr& obj, unsigned length);
		void ProcessDefineShape(const character_def_ptr& obj, unsigned length);
		void ProcessPlaceObject(const character_def_ptr& obj, unsigned length);
		void ProcessRemoveObject(const character_def_ptr& obj, unsigned length);
		void ProcessDefineBits(const character_def_ptr& obj, unsigned length);
		void ProcessDefineButton(const character_def_ptr& obj, unsigned length);
		void ProcessJPEGTables(const character_def_ptr& obj, unsigned length);
		void ProcessSetBackgroundColor(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFont(const character_def_ptr& obj, unsigned length);
		void ProcessDefineText(const character_def_ptr& obj, unsigned length);
		void ProcessDoAction(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFontInfo(const character_def_ptr& obj, unsigned length);
		void ProcessDefineSound(const character_def_ptr& obj, unsigned length);
		void ProcessStartSound(const character_def_ptr& obj, unsigned length);
		void ProcessDefineButtonSound(const character_def_ptr& obj, unsigned length);
		void ProcessSoundStreamHead(const character_def_ptr& obj, unsigned length);
		void ProcessSoundStreamBlock(const character_def_ptr& obj, unsigned length);
		void ProcessBitsLossless(const character_def_ptr& obj, unsigned length);
		void ProcessDefineBitsJPEG2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineShape2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineButtonCxform(const character_def_ptr& obj, unsigned length);
		void ProcessProtect(const character_def_ptr& obj, unsigned length);
		void ProcessPlaceObject2(const character_def_ptr& obj, unsigned length);
		void ProcessRemoveObject2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineShape3(const character_def_ptr& obj, unsigned length);
		void ProcessDefineText2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineButton2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineBitsJPEG3(const character_def_ptr& obj, unsigned length);
		void ProcessDefineBitsLossless2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineEditText(const character_def_ptr& obj, unsigned length);
		void ProcessDefineSprite(const character_def_ptr& obj, unsigned length);
		void ProcessFrameLabel(const character_def_ptr& obj, unsigned length);
		void ProcessSoundStreamHead2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineMorphShape(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFont2(const character_def_ptr& obj, unsigned length);
		void ProcessExportAssets(const character_def_ptr& obj, unsigned length);
		void ProcessImportAssets(const character_def_ptr& obj, unsigned length);
		void ProcessEnableDebugger(const character_def_ptr& obj, unsigned length);
		void ProcessDoInitAction(const character_def_ptr& obj, unsigned length);
		void ProcessDefineVideoStream(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFontInfo2(const character_def_ptr& obj, unsigned length);
		void ProcessEnableDebugger2(const character_def_ptr& obj, unsigned length);
		void ProcessScriptLimits(const character_def_ptr& obj, unsigned length);
		void ProcessSetTabIndex(const character_def_ptr& obj, unsigned length);
		void ProcessFileAttributes(const character_def_ptr& obj, unsigned length);
		void ProcessPlaceObject3(const character_def_ptr& obj, unsigned length);
		void ProcessImportAssets2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFontAlignZones(const character_def_ptr& obj, unsigned length);
		void ProcessDefineCSMTextSettings(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFont3(const character_def_ptr& obj, unsigned length);
		void ProcessSymbolClass(const character_def_ptr& obj, unsigned length);
		void ProcessMetadata(const character_def_ptr& obj, unsigned length);
		void ProcessDefineScalingGrid(const character_def_ptr& obj, unsigned length);
		void ProcessDoABC(const character_def_ptr& obj, unsigned length);
		void ProcessDefineShape4(const character_def_ptr& obj, unsigned length);
		void ProcessDefineMorphShape2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineSceneAndFrameLabelData(const character_def_ptr& obj, unsigned length);
		void ProcessDefineBinaryData(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFontName(const character_def_ptr& obj, unsigned length);
		void ProcessStartSound2(const character_def_ptr& obj, unsigned length);
		void ProcessDefineJPEGBits4(const character_def_ptr& obj, unsigned length);
		void ProcessDefineFont4(const character_def_ptr& obj, unsigned length);
		void ProcessEnableTelemetry(const character_def_ptr& obj, unsigned length);

		void read_tags(const character_def_ptr& obj);
		void eat_bit_stream(unsigned length);

		reader();
		reader(const reader&);
	};
}
