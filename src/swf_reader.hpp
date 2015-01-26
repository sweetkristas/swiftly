#pragma once

#include <stack>
#include <string>
#include <zlib.h>
#include "bit_reader.hpp"
#include "swf.hpp"
#include "swf_movie.hpp"

namespace swf
{
	class reader
	{
	public:
		explicit reader(const std::string& fname, movie& obj);

		void push_indent(int n);
		void pop_indent();
		int get_indent() const { return indent_; }
		std::string get_indent_value() const { return std::string(indent_, ' '); }
	private:
		std::stack<int> indent_stack_;
		int indent_;
		std::shared_ptr<bit_stream> bits_;

		void ProcessShowFrame(movie& obj, unsigned length);
		void ProcessDefineShape(movie& obj, unsigned length);
		void ProcessPlaceObject(movie& obj, unsigned length);
		void ProcessRemoveObject(movie& obj, unsigned length);
		void ProcessDefineBits(movie& obj, unsigned length);
		void ProcessDefineButton(movie& obj, unsigned length);
		void ProcessJPEGTables(movie& obj, unsigned length);
		void ProcessSetBackgroundColor(movie& obj, unsigned length);
		void ProcessDefineFont(movie& obj, unsigned length);
		void ProcessDefineText(movie& obj, unsigned length);
		void ProcessDoAction(movie& obj, unsigned length);
		void ProcessDefineFontInfo(movie& obj, unsigned length);
		void ProcessDefineSound(movie& obj, unsigned length);
		void ProcessStartSound(movie& obj, unsigned length);
		void ProcessDefineButtonSound(movie& obj, unsigned length);
		void ProcessSoundStreamHead(movie& obj, unsigned length);
		void ProcessSoundStreamBlock(movie& obj, unsigned length);
		void ProcessBitsLossless(movie& obj, unsigned length);
		void ProcessDefineBitsJPEG2(movie& obj, unsigned length);
		void ProcessDefineShape2(movie& obj, unsigned length);
		void ProcessDefineButtonCxform(movie& obj, unsigned length);
		void ProcessProtect(movie& obj, unsigned length);
		void ProcessPlaceObject2(movie& obj, unsigned length);
		void ProcessRemoveObject2(movie& obj, unsigned length);
		void ProcessDefineShape3(movie& obj, unsigned length);
		void ProcessDefineText2(movie& obj, unsigned length);
		void ProcessDefineButton2(movie& obj, unsigned length);
		void ProcessDefineBitsJPEG3(movie& obj, unsigned length);
		void ProcessDefineBitsLossless2(movie& obj, unsigned length);
		void ProcessDefineEditText(movie& obj, unsigned length);
		void ProcessDefineSprite(movie& obj, unsigned length);
		void ProcessFrameLabel(movie& obj, unsigned length);
		void ProcessSoundStreamHead2(movie& obj, unsigned length);
		void ProcessDefineMorphShape(movie& obj, unsigned length);
		void ProcessDefineFont2(movie& obj, unsigned length);
		void ProcessExportAssets(movie& obj, unsigned length);
		void ProcessImportAssets(movie& obj, unsigned length);
		void ProcessEnableDebugger(movie& obj, unsigned length);
		void ProcessDoInitAction(movie& obj, unsigned length);
		void ProcessDefineVideoStream(movie& obj, unsigned length);
		void ProcessDefineFontInfo2(movie& obj, unsigned length);
		void ProcessEnableDebugger2(movie& obj, unsigned length);
		void ProcessScriptLimits(movie& obj, unsigned length);
		void ProcessSetTabIndex(movie& obj, unsigned length);
		void ProcessFileAttributes(movie& obj, unsigned length);
		void ProcessPlaceObject3(movie& obj, unsigned length);
		void ProcessImportAssets2(movie& obj, unsigned length);
		void ProcessDefineFontAlignZones(movie& obj, unsigned length);
		void ProcessDefineCSMTextSettings(movie& obj, unsigned length);
		void ProcessDefineFont3(movie& obj, unsigned length);
		void ProcessSymbolClass(movie& obj, unsigned length);
		void ProcessMetadata(movie& obj, unsigned length);
		void ProcessDefineScalingGrid(movie& obj, unsigned length);
		void ProcessDoABC(movie& obj, unsigned length);
		void ProcessDefineShape4(movie& obj, unsigned length);
		void ProcessDefineMorphShape2(movie& obj, unsigned length);
		void ProcessDefineSceneAndFrameLabelData(movie& obj, unsigned length);
		void ProcessDefineBinaryData(movie& obj, unsigned length);
		void ProcessDefineFontName(movie& obj, unsigned length);
		void ProcessStartSound2(movie& obj, unsigned length);
		void ProcessDefineJPEGBits4(movie& obj, unsigned length);
		void ProcessDefineFont4(movie& obj, unsigned length);
		void ProcessEnableTelemetry(movie& obj, unsigned length);

		void read_tags(movie& obj);
		void eat_bit_stream(unsigned length);

		reader();
		reader(const reader&);
	};
}
