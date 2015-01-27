#pragma once

#include <stack>
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

		void push_indent(int n);
		void pop_indent();
		int get_indent() const { return indent_; }
		std::string get_indent_value() const { return std::string(indent_, ' '); }
	private:
		std::stack<int> indent_stack_;
		int indent_;
		std::shared_ptr<bit_stream> bits_;

		void ProcessShowFrame(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineShape(const movie_def_ptr& obj, unsigned length);
		void ProcessPlaceObject(const movie_def_ptr& obj, unsigned length);
		void ProcessRemoveObject(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineBits(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineButton(const movie_def_ptr& obj, unsigned length);
		void ProcessJPEGTables(const movie_def_ptr& obj, unsigned length);
		void ProcessSetBackgroundColor(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFont(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineText(const movie_def_ptr& obj, unsigned length);
		void ProcessDoAction(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFontInfo(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineSound(const movie_def_ptr& obj, unsigned length);
		void ProcessStartSound(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineButtonSound(const movie_def_ptr& obj, unsigned length);
		void ProcessSoundStreamHead(const movie_def_ptr& obj, unsigned length);
		void ProcessSoundStreamBlock(const movie_def_ptr& obj, unsigned length);
		void ProcessBitsLossless(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineBitsJPEG2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineShape2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineButtonCxform(const movie_def_ptr& obj, unsigned length);
		void ProcessProtect(const movie_def_ptr& obj, unsigned length);
		void ProcessPlaceObject2(const movie_def_ptr& obj, unsigned length);
		void ProcessRemoveObject2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineShape3(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineText2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineButton2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineBitsJPEG3(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineBitsLossless2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineEditText(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineSprite(const movie_def_ptr& obj, unsigned length);
		void ProcessFrameLabel(const movie_def_ptr& obj, unsigned length);
		void ProcessSoundStreamHead2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineMorphShape(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFont2(const movie_def_ptr& obj, unsigned length);
		void ProcessExportAssets(const movie_def_ptr& obj, unsigned length);
		void ProcessImportAssets(const movie_def_ptr& obj, unsigned length);
		void ProcessEnableDebugger(const movie_def_ptr& obj, unsigned length);
		void ProcessDoInitAction(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineVideoStream(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFontInfo2(const movie_def_ptr& obj, unsigned length);
		void ProcessEnableDebugger2(const movie_def_ptr& obj, unsigned length);
		void ProcessScriptLimits(const movie_def_ptr& obj, unsigned length);
		void ProcessSetTabIndex(const movie_def_ptr& obj, unsigned length);
		void ProcessFileAttributes(const movie_def_ptr& obj, unsigned length);
		void ProcessPlaceObject3(const movie_def_ptr& obj, unsigned length);
		void ProcessImportAssets2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFontAlignZones(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineCSMTextSettings(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFont3(const movie_def_ptr& obj, unsigned length);
		void ProcessSymbolClass(const movie_def_ptr& obj, unsigned length);
		void ProcessMetadata(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineScalingGrid(const movie_def_ptr& obj, unsigned length);
		void ProcessDoABC(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineShape4(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineMorphShape2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineSceneAndFrameLabelData(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineBinaryData(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFontName(const movie_def_ptr& obj, unsigned length);
		void ProcessStartSound2(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineJPEGBits4(const movie_def_ptr& obj, unsigned length);
		void ProcessDefineFont4(const movie_def_ptr& obj, unsigned length);
		void ProcessEnableTelemetry(const movie_def_ptr& obj, unsigned length);

		void read_tags(const movie_def_ptr& obj);
		void eat_bit_stream(unsigned length);

		reader();
		reader(const reader&);
	};
}
