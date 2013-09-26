#include <map>
#include <sstream>
#include "asserts.hpp"
#include "avm2.hpp"
#include "decompress.hpp"
#include "swf_button.hpp"
#include "swf_image.hpp"
#include "swf_reader.hpp"
#include "swf_font.hpp"
#include "swf_shape.hpp"
#include "swf_text.hpp"

namespace swf
{
	const uint16_t LongRecordHeader = 0x3f;

	namespace
	{
		const std::vector<uint8_t> read_file(const std::string& fname)
		{
			std::ifstream file(fname.c_str(), std::ios_base::binary);
			file.seekg(0, file.end);
			std::streamoff length = file.tellg();
			file.seekg(0, file.beg);
			ASSERT_LOG(length > 0, "File '" + fname + "' read had zero length");

			std::vector<char> v;
			v.resize(size_t(length));
			file.read(&v[0], length);
			
			return std::vector<uint8_t>(v.begin(), v.end());
		}

		std::map<reader::tagName, std::string>& get_tag_name()
		{
			static std::map<reader::tagName, std::string> result;
			if(result.empty()) {
				result[reader::TAG_END] = "End";
				result[reader::TAG_SHOW_FRAME] = "ShowFrame";
				result[reader::TAG_DEFINE_SHAPE] = "DefineShape";
				result[reader::TAG_PLACE_OBJECT] = "PlaceObject";
				result[reader::TAG_REMOVE_OBJECT] = "RemoveObject";
				result[reader::TAG_DEFINE_BITS] = "DefineBits";
				result[reader::TAG_DEFINE_BUTTON] = "DefineButton";
				result[reader::TAG_JPEG_TABLES] = "JPEGTables";
				result[reader::TAG_SET_BACKGROUND_COLOR] = "SetBackgroundColor";
				result[reader::TAG_DEFINE_FONT] = "DefineFont";
				result[reader::TAG_DEFINE_TEXT] = "DefineText";
				result[reader::TAG_DO_ACTION] = "DoAction";
				result[reader::TAG_DEFINE_FONT_INFO] = "DefineFontInfo";
				result[reader::TAG_DEFINE_SOUND] = "DefineSound";
				result[reader::TAG_START_SOUND] = "StartSound";
				result[reader::TAG_DEFINE_BUTTON_SOUND] = "DefineButtonSound";
				result[reader::TAG_SOUND_STREAM_HEAD] = "SoundStreamHead";
				result[reader::TAG_SOUND_STREAM_BLOCK] = "SoundStreamBlock";
				result[reader::TAG_DEFINE_BITS_LOSSLESS] = "BitsLossless";
				result[reader::TAG_DEFINE_BITS_JPEG2] = "DefineBitsJPEG2";
				result[reader::TAG_DEFINE_SHAPE2] = "DefineShape2";
				result[reader::TAG_DEFINE_BUTTON_CXFORM] = "DefineButtonCxform";
				result[reader::TAG_PROTECT] = "Protext";
				result[reader::TAG_PLACE_OBJECT2] = "PlaceObject2";
				result[reader::TAG_REMOVE_OBJECT2] = "RemoveObject2";
				result[reader::TAG_DEFINE_SHAPE3] = "DefineShape3";
				result[reader::TAG_DEFINE_TEXT2] = "DefineText2";
				result[reader::TAG_DEFINE_BUTTON2] = "DefineButton2";
				result[reader::TAG_DEFINE_BITS_JPEG3] = "DefineBitsJPEG3";
				result[reader::TAG_DEFINE_BITS_LOSSLESS2] = "DefineBitsLossless2";
				result[reader::TAG_DEFINE_EDIT_TEXT] = "DefineEditText";
				result[reader::TAG_DEFINE_SPRITE] = "DefineSprite";
				result[reader::TAG_FRAME_LABEL] = "FrameLabel";
				result[reader::TAG_SOUND_STREAM_HEAD2] = "SoundStreamHead2";
				result[reader::TAG_DEFINE_MORPH_SHAPE] = "DefineMorphShape";
				result[reader::TAG_DEFINE_FONT2] = "DefineFont2";
				result[reader::TAG_EXPORT_ASSETS] = "ExportAssets";
				result[reader::TAG_IMPORT_ASSETS] = "ImportAssets";
				result[reader::TAG_ENABLE_DEBUGGER] = "EnableDebugger";
				result[reader::TAG_DO_INIT_ACTION] = "DoInitAction";
				result[reader::TAG_DEFINE_VIDEO_STREAM] = "DefineVideoStream";
				result[reader::TAG_DEFINE_FONT_INFO2] = "DefineFontInfo2";
				result[reader::TAG_ENABLE_DEBUGGER2] = "EnableDebugger2";
				result[reader::TAG_SCRIPT_LIMITS] = "ScriptLimits";
				result[reader::TAG_SET_TAB_INDEX] = "SetTabIndex";
				result[reader::TAG_FILE_ATTRIBUTES] = "FileAttributes";
				result[reader::TAG_PLACE_OBJECT3] = "PlaceObject3";
				result[reader::TAG_IMPORT_ASSETS2] = "ImportAssets2";
				result[reader::TAG_DEFINE_FONT_ALIGN_ZONES] = "DefineFontAlignZones";
				result[reader::TAG_DEFINE_CSM_TEXT_SETTINGS] = "DefineCSMTextSettings";
				result[reader::TAG_DEFINE_FONT3] = "DefineFont3";
				result[reader::TAG_SYMBOL_CLASS] = "SymbolClass";
				result[reader::TAG_METADATA] = "Metadata";
				result[reader::TAG_DEFINE_SCALING_GRID] = "DefineScalingGrid";
				result[reader::TAG_DO_ABC] = "DoABC";
				result[reader::TAG_DEFINE_SHAPE4] = "DefineShape4";
				result[reader::TAG_DEFINE_MORPH_SHAPE2] = "DefineMorphShape2";
				result[reader::TAG_DEFINE_SCENE_AND_FRAME_LABEL_DATA] = "DefineSceneAndFrameLabelData";
				result[reader::TAG_DEFINE_BINARY_DATA] = "DefineBinaryData";
				result[reader::TAG_DEFINE_FONT_NAME] = "DefineFontName";
				result[reader::TAG_START_SOUND2] = "StartSound2";
				result[reader::TAG_DEFINE_JPEG_BITS4] = "DefineJPEGBits4";
				result[reader::TAG_DEFINE_FONT4] = "DefineFont4";
				result[reader::TAG_ENABLE_TELEMETRY] = "EnableTelemetry";
			}
			return result;
		}
	}

	reader::reader(const std::string& fname, swf& obj) : obj_(obj)
	{
		std::vector<uint8_t> bits = read_file(fname);
		bits_.reset(new bit_stream(bits));
		char s1 = bits_->read_signed8();
		char s2 = bits_->read_signed8();
		char s3 = bits_->read_signed8();
		uint8_t swf_version = bits_->read_unsigned8();
		bits_->set_swf_version(swf_version);
		uint32_t file_length = bits_->read_unsigned32();
		std::cerr << fname << ", Signature: " << s1 << s2 << s3 << ", Version: " << int(swf_version) << ", File Length: " << file_length << std::endl;

		if(s1 == 'F') {
			// uncompressed data, do nothing
			ASSERT_LOG(bits_->size() == file_length, "File length different from read data. " << bits_->size() << " : " << file_length);
		} else if(s1 == 'C') {
			// zlib compressed
			bits_.reset(new bit_stream(zip::decompress(&bits[8], bits.size()-8)));
			bits_->set_swf_version(swf_version);
			ASSERT_LOG(bits_->size()+8 == file_length, "File length different from read data. " << (bits_->size()+8) << " : " << file_length);
		} else if(s1 == 'Z') {
			// lzma compressed
			ASSERT_LOG(false, "LZMA compression not currently supported.");
		}

		geometry::rect frame_size = bits_->read_rect();
		fixed_point frame_rate = bits_->read_fixedpoint_bits(16);
		uint16_t frame_count = bits_->read_unsigned16();

		std::cerr << "Frame Size: " << frame_size.x1/20.0 << "," << frame_size.y1/20.0 << "," << frame_size.x2/20.0 << "," << frame_size.y2/20.0 << std::endl;
		std::cerr << "Frame Rate: " << frame_rate.to_double() << ", Frame Count: " << frame_count << std::endl;

		obj.set_version(swf_version);
		obj.set_frame_size(frame_size);
		obj.set_frame_rate(frame_rate);
		obj.set_frame_count(frame_count);

		read_tags();
	}

	void reader::read_tags()
	{
		// Read tagged blocks until we get the end block.
		bool finished = false;
		while(!finished) {
			uint16_t tag_code_and_length = bits_->read_unsigned16();
			unsigned tag_code = tag_code_and_length >> 6;
			unsigned length = tag_code_and_length & ((1 << 6)-1);

			if(length == LongRecordHeader) {
				length = bits_->read_unsigned32();
				auto it = get_tag_name().find(enum tagName(tag_code));
				ASSERT_LOG(it != get_tag_name().end(), "Tag Code: " << tag_code << " not found.");
				std::cerr << "Long tag: " << it->second << std::endl;
			} else {
				auto it = get_tag_name().find(enum tagName(tag_code));
				ASSERT_LOG(it != get_tag_name().end(), "Tag Code: " << tag_code << " not found.");
				std::cerr << "Short tag: " << it->second << std::endl;
			}
			switch(tag_code) {
			case TAG_END: finished = true; break;
			case TAG_SHOW_FRAME:                        ProcessShowFrame(length); break;
			case TAG_DEFINE_SHAPE:                      ProcessDefineShape(length); break;
			case TAG_PLACE_OBJECT:                      ProcessPlaceObject(length); break;
			case TAG_REMOVE_OBJECT:                     ProcessRemoveObject(length); break;
			case TAG_DEFINE_BITS:                       ProcessDefineBits(length); break;
			case TAG_DEFINE_BUTTON:                     ProcessDefineButton(length); break;
			case TAG_JPEG_TABLES:                       ProcessJPEGTables(length); break;
			case TAG_SET_BACKGROUND_COLOR:              ProcessSetBackgroundColor(length); break;
			case TAG_DEFINE_FONT:                       ProcessDefineFont(length); break;
			case TAG_DEFINE_TEXT:                       ProcessDefineText(length); break;
			case TAG_DO_ACTION:                         ProcessDoAction(length); break;
			case TAG_DEFINE_FONT_INFO:                  ProcessDefineFontInfo(length); break;
			case TAG_DEFINE_SOUND:                      ProcessDefineSound(length); break;
			case TAG_START_SOUND:                       ProcessStartSound(length); break;
			case TAG_DEFINE_BUTTON_SOUND:               ProcessDefineButtonSound(length); break;
			case TAG_SOUND_STREAM_HEAD:                 ProcessSoundStreamHead(length); break;
			case TAG_SOUND_STREAM_BLOCK:                ProcessSoundStreamBlock(length); break;
			case TAG_DEFINE_BITS_LOSSLESS:              ProcessBitsLossless(length); break;
			case TAG_DEFINE_BITS_JPEG2:                 ProcessDefineBitsJPEG2(length); break;
			case TAG_DEFINE_SHAPE2:                     ProcessDefineShape2(length); break;
			case TAG_DEFINE_BUTTON_CXFORM:              ProcessDefineButtonCxform(length); break;
			case TAG_PROTECT:                           ProcessProtect(length); break;
			case TAG_PLACE_OBJECT2:                     ProcessPlaceObject2(length); break;
			case TAG_REMOVE_OBJECT2:                    ProcessRemoveObject2(length); break;
			case TAG_DEFINE_SHAPE3:                     ProcessDefineShape3(length); break;
			case TAG_DEFINE_TEXT2:                      ProcessDefineText2(length); break;
			case TAG_DEFINE_BUTTON2:                    ProcessDefineButton2(length); break;
			case TAG_DEFINE_BITS_JPEG3:                 ProcessDefineBitsJPEG3(length); break;
			case TAG_DEFINE_BITS_LOSSLESS2:             ProcessDefineBitsLossless2(length); break;
			case TAG_DEFINE_EDIT_TEXT:                  ProcessDefineEditText(length); break;
			case TAG_DEFINE_SPRITE:                     ProcessDefineSprite(length); break;
			case TAG_FRAME_LABEL:                       ProcessFrameLabel(length); break;
			case TAG_SOUND_STREAM_HEAD2:                ProcessSoundStreamHead2(length); break;
			case TAG_DEFINE_MORPH_SHAPE:                ProcessDefineMorphShape(length); break;
			case TAG_DEFINE_FONT2:                      ProcessDefineFont2(length); break;
			case TAG_EXPORT_ASSETS:                     ProcessExportAssets(length); break;
			case TAG_IMPORT_ASSETS:                     ProcessImportAssets(length); break;
			case TAG_ENABLE_DEBUGGER:                   ProcessEnableDebugger(length); break;
			case TAG_DO_INIT_ACTION:                    ProcessDoInitAction(length); break;
			case TAG_DEFINE_VIDEO_STREAM:               ProcessDefineVideoStream(length); break;
			case TAG_DEFINE_FONT_INFO2:                 ProcessDefineFontInfo2(length); break;
			case TAG_ENABLE_DEBUGGER2:                  ProcessEnableDebugger2(length); break;
			case TAG_SCRIPT_LIMITS:                     ProcessScriptLimits(length); break;
			case TAG_SET_TAB_INDEX:                     ProcessSetTabIndex(length); break;
			case TAG_FILE_ATTRIBUTES:                   ProcessFileAttributes(length); break;
			case TAG_PLACE_OBJECT3:                     ProcessPlaceObject3(length); break;
			case TAG_IMPORT_ASSETS2:                    ProcessImportAssets2(length); break;
			case TAG_DEFINE_FONT_ALIGN_ZONES:           ProcessDefineFontAlignZones(length); break;
			case TAG_DEFINE_CSM_TEXT_SETTINGS:          ProcessDefineCSMTextSettings(length); break;
			case TAG_DEFINE_FONT3:                      ProcessDefineFont3(length); break;
			case TAG_SYMBOL_CLASS:                      ProcessSymbolClass(length); break;
			case TAG_METADATA:                          ProcessMetadata(length); break;
			case TAG_DEFINE_SCALING_GRID:               ProcessDefineScalingGrid(length); break;
			case TAG_DO_ABC:                            ProcessDoABC(length); break;
			case TAG_DEFINE_SHAPE4:                     ProcessDefineShape4(length); break;
			case TAG_DEFINE_MORPH_SHAPE2:               ProcessDefineMorphShape2(length); break;
			case TAG_DEFINE_SCENE_AND_FRAME_LABEL_DATA: ProcessDefineSceneAndFrameLabelData(length); break;
			case TAG_DEFINE_BINARY_DATA:                ProcessDefineBinaryData(length); break;
			case TAG_DEFINE_FONT_NAME:                  ProcessDefineFontName(length); break;
			case TAG_START_SOUND2:                      ProcessStartSound2(length); break;
			case TAG_DEFINE_JPEG_BITS4:                 ProcessDefineJPEGBits4(length); break;
			case TAG_DEFINE_FONT4:                      ProcessDefineFont4(length); break;
			case TAG_ENABLE_TELEMETRY:                  ProcessEnableTelemetry(length); break;
			}
		}
	}

	void reader::eat_bit_stream(unsigned length)
	{
		// For tags not processed call this function to remove data from the bit stream
		while(length--) {
			bits_->read_unsigned8();
		}
	}

	void reader::ProcessShowFrame(unsigned length)
	{
		obj_.draw();
	}


	void reader::ProcessDefineShape(unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(1, bits_);
		obj_.add_character(s->id(), s);
	}


	void reader::ProcessPlaceObject(unsigned length)
	{
		eat_bit_stream(length);
		std::cerr << "Ignored PlaceObject tag" << std::endl;
	}


	void reader::ProcessRemoveObject(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'RemoveObject'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineBits(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineBits'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButton(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButton'");
		eat_bit_stream(length);
	}


	void reader::ProcessJPEGTables(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'JPEGTables'");
		eat_bit_stream(length);
	}


	void reader::ProcessSetBackgroundColor(unsigned length)
	{
		obj_.set_background_color(bits_->read_rgb());
	}


	void reader::ProcessDefineFont(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineText(unsigned length)
	{
		text* t = new text;
		t->read(bits_);
		// Add text to character list.
		obj_.add_character(t->id(), t);
	}


	void reader::ProcessDoAction(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DoAction'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontInfo(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFontInfo'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSound(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessStartSound(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'StartSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButtonSound(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButtonSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamHead(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamHead'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamBlock(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamBlock'");
		eat_bit_stream(length);
	}


	void reader::ProcessBitsLossless(unsigned length)
	{
		image_character* img = new image_character(1, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read_lossless(bits_);
		obj_.add_character(img->id(), img);
	}


	void reader::ProcessDefineBitsJPEG2(unsigned length)
	{
		image_character* img = new image_character(2, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read(bits_);
		obj_.add_character(img->id(), img);
	}


	void reader::ProcessDefineShape2(unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(2, bits_);
		obj_.add_character(s->id(), s);
	}


	void reader::ProcessDefineButtonCxform(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButtonCxform'");
		eat_bit_stream(length);
	}


	void reader::ProcessProtect(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'Protect'");
		eat_bit_stream(length);
	}


	void reader::ProcessPlaceObject2(unsigned length)
	{
		bool has_clip_actions = bits_->read_unsigned_bits(1) ? true : false;
		bool has_clip_depth = bits_->read_unsigned_bits(1) ? true : false;
		bool has_name = bits_->read_unsigned_bits(1) ? true : false;
		bool has_ratio = bits_->read_unsigned_bits(1) ? true : false;
		bool has_color_transform = bits_->read_unsigned_bits(1) ? true : false;
		bool has_matrix = bits_->read_unsigned_bits(1) ? true : false;
		bool has_character = bits_->read_unsigned_bits(1) ? true : false;
		bool move = bits_->read_unsigned_bits(1) ? true : false;
		uint16_t depth = bits_->read_unsigned16();

		displayable_ptr display_instance;

		if(has_character) {
			display_instance = displayable_ptr(new displayable(obj_.find_character(bits_->read_unsigned16())));
		} else {
			display_instance = obj_.find_displayable(depth);
		}

		if(has_matrix) {
			display_instance->set_transform(bits_->read_matrix());
		}
		if(has_color_transform) {
			display_instance->set_cxtransform(bits_->read_cxform_with_alpha());
		}
		if(has_ratio) {
			display_instance->set_ratio(bits_->read_unsigned16());
		}
		if(has_name) {
			display_instance->set_name(bits_->read_string());
		}
		if(has_clip_depth) {
			display_instance->set_clip_depth(bits_->read_unsigned16());
		}
		if(has_clip_actions) {
			// XXX
			bits_->read_clip_actions();
			ASSERT_LOG(false, "clip actions not supported.");
		}

		if(has_character) {
			if(move) {
				obj_.remove_character_from_display_list(depth);
			}
			obj_.add_character_to_display_list(depth, display_instance);
		}
	}


	void reader::ProcessRemoveObject2(unsigned length)
	{
		uint16_t depth = bits_->read_unsigned16();
		obj_.remove_character_from_display_list(depth);
	}


	void reader::ProcessDefineShape3(unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(3, bits_);
		obj_.add_character(s->id(), s);
	}


	void reader::ProcessDefineText2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineText2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButton2(unsigned length)
	{
		button* b = new button;
		b->set_id(bits_->read_unsigned16());
		b->read2(bits_);
		// Add text to character list.
		obj_.add_character(b->id(), b);		
	}


	void reader::ProcessDefineBitsJPEG3(unsigned length)
	{
		image_character* img = new image_character(3, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read(bits_);
		obj_.add_character(img->id(), img);
	}


	void reader::ProcessDefineBitsLossless2(unsigned length)
	{
		image_character* img = new image_character(2, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read_lossless(bits_);
		obj_.add_character(img->id(), img);
	}


	void reader::ProcessDefineEditText(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineEditText'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSprite(unsigned length)
	{
		uint16_t sprite_id = bits_->read_unsigned16();
		uint16_t sprite_frame_count = bits_->read_unsigned16();
		read_tags();
	}


	void reader::ProcessFrameLabel(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'FrameLabel'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamHead2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamHead2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineMorphShape(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineMorphShape'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFont2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont2'");
		eat_bit_stream(length);
	}


	void reader::ProcessExportAssets(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ExportAssets'");
		eat_bit_stream(length);
	}


	void reader::ProcessImportAssets(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ImportAssets'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableDebugger(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'EnableDebugger'");
		eat_bit_stream(length);
	}


	void reader::ProcessDoInitAction(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DoInitAction'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineVideoStream(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineVideoStream'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontInfo2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFontInfo2'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableDebugger2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'EnableDebugger2'");
		eat_bit_stream(length);
	}


	void reader::ProcessScriptLimits(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ScriptLimits'");
		eat_bit_stream(length);
	}


	void reader::ProcessSetTabIndex(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SetTabIndex'");
		eat_bit_stream(length);
	}


	void reader::ProcessFileAttributes(unsigned length)
	{
		bits_->read_unsigned_bits(1); // reserved
		bool use_direct_blit = bits_->read_unsigned_bits(1) ? true : false;
		bool use_gpu = bits_->read_unsigned_bits(1) ? true : false;
		bool has_metadata = bits_->read_unsigned_bits(1) ? true : false;
		bool use_as3 = bits_->read_unsigned_bits(1) ? true : false;
		bits_->read_unsigned_bits(2); // reserved
		bool use_network = bits_->read_unsigned_bits(1) ? true : false;
		bits_->read_unsigned_bits(24); // reserved
		obj_.set_attributes(use_direct_blit, use_gpu, use_as3, use_network);
	}


	void reader::ProcessPlaceObject3(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'PlaceObject3'");
		eat_bit_stream(length);
	}


	void reader::ProcessImportAssets2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ImportAssets2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontAlignZones(unsigned length)
	{
		uint16_t font_id = bits_->read_unsigned16();
		unsigned csm_table_hint = bits_->read_unsigned_bits(2);
		bits_->read_unsigned_bits(6); // reserved

		font_ptr f = boost::dynamic_pointer_cast<font>(obj_.find_character(font_id));
		ASSERT_LOG(f != NULL, "Couldn't convert character at position " << font_id << " to a font.");
		f->set_csm_hint(csm_table_hint);
		std::vector<zone_record> zrs;
		for(size_t n = 0; n != f->size(); ++n) {
			zone_record zr;
			unsigned num_zone_data = bits_->read_unsigned8();
			ASSERT_LOG(num_zone_data == 2, "NumZoneData expected to be 2 in specification.");
			zr.zone_data[0].alignment_coordinate = bits_->read_half_float();
			zr.zone_data[0].range = bits_->read_half_float();
			zr.zone_data[1].alignment_coordinate = bits_->read_half_float();
			zr.zone_data[1].range = bits_->read_half_float();
			bits_->read_unsigned_bits(6); // reserved
			zr.zone_mask_x = bits_->read_unsigned_bits(1) ? true : false;
			zr.zone_mask_y = bits_->read_unsigned_bits(1) ? true : false;
			zrs.push_back(zr);
		}
		f->set_zone_records(&zrs);
	}


	void reader::ProcessDefineCSMTextSettings(unsigned length)
	{
		uint16_t text_id = bits_->read_unsigned16();
		text_ptr t = boost::dynamic_pointer_cast<text>(obj_.find_character(text_id));
		ASSERT_LOG(t != NULL, "Couldn't convert character with id: " << text_id << " to a text entity");
		t->set_render_type(bits_->read_unsigned_bits(2));
		t->set_grid_fit(bits_->read_unsigned_bits(3));
		bits_->read_unsigned_bits(3); // reserved
		t->set_thickness(bits_->read_float());
		t->set_sharpness(bits_->read_float());
		bits_->read_unsigned8(); // reserved
	}


	void reader::ProcessDefineFont3(unsigned length)
	{
		font* f = new font;
		f->read3(bits_);
		// Add front to character list.
		obj_.add_character(f->id(), f);
	}


	void reader::ProcessSymbolClass(unsigned length)
	{
		uint32_t num_syms = uint32_t(bits_->read_unsigned16());
		for(uint32_t n = 0; n != num_syms; ++n) {
			uint16_t tag = bits_->read_unsigned16();
			std::string name = bits_->read_string();
			obj_.add_symbol_class(tag, name);
		}
	}


	void reader::ProcessMetadata(unsigned length)
	{
		// deliberately ignore the metadata tag.
		std::cerr << "Ignoring metadata tag" << std::endl;
		eat_bit_stream(length);
	}


	void reader::ProcessDefineScalingGrid(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineScalingGrid'");
		eat_bit_stream(length);
	}


	void reader::ProcessDoABC(unsigned length)
	{
		uint32_t flags = bits_->read_unsigned32();
		std::string name = bits_->read_string();
		obj_.add_abc(std::shared_ptr<avm2::abc_file>(new avm2::abc_file(flags, name, bits_)));
	}


	void reader::ProcessDefineShape4(unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(4, bits_);
		obj_.add_character(s->id(), s);
	}


	void reader::ProcessDefineMorphShape2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineMorphShape2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSceneAndFrameLabelData(unsigned length)
	{
		uint32_t num_scenes = bits_->read_unsigned32_encoded();
		scene_info scene;
		for(int n = 0; n != num_scenes; ++n) {
			uint32_t scene_frame_offset = bits_->read_unsigned32_encoded();
			std::string scene_name = bits_->read_string();
			scene.push_back(std::make_pair(scene_frame_offset, scene_name));
		}

		uint32_t num_frame_labels = bits_->read_unsigned32_encoded();
		frame_label_info frame_label;
		for(int n = 0; n != num_frame_labels; ++n) {
			uint32_t frame_num = bits_->read_unsigned32_encoded();
			std::string frame_label_str = bits_->read_string();
			frame_label.push_back(std::make_pair(frame_num, frame_label_str));
		}

		obj_.set_scene_and_frame_info(scene, frame_label);
	}


	void reader::ProcessDefineBinaryData(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineBinaryData'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontName(unsigned length)
	{
		std::cerr << "Ignoring DefineFontName tag" << std::endl;
		eat_bit_stream(length);
	}


	void reader::ProcessStartSound2(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'StartSound2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineJPEGBits4(unsigned length)
	{
		image_character* img = new image_character(4, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read(bits_);
		obj_.add_character(img->id(), img);
	}


	void reader::ProcessDefineFont4(unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont4'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableTelemetry(unsigned length)
	{
		std::cerr << "Ignoring 'EnableTelemetry' tag" << std::endl;
		eat_bit_stream(length);
	}

}
