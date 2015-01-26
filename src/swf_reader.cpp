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
#include "swf_tags.hpp"
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
	}

	reader::reader(const std::string& fname, movie& obj) : indent_(0)
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

		push_indent(2);
		read_tags(obj);
		pop_indent();
	}

	void reader::push_indent(int n)
	{
		indent_stack_.push(indent_);
		indent_ = n;
	}

	void reader::pop_indent()
	{
		indent_ = indent_stack_.top();
		indent_stack_.pop();
	}

	void reader::read_tags(movie& obj)
	{
		// Read tagged blocks until we get the end block.
		bool finished = false;
		while(!finished) {
			uint16_t tag_code_and_length = bits_->read_unsigned16();
			unsigned code = tag_code_and_length >> 6;
			Tag tag_code = static_cast<Tag>(code);
			unsigned length = tag_code_and_length & ((1 << 6)-1);

			if(length == LongRecordHeader) {
				length = bits_->read_unsigned32();
			}
			LOG_DEBUG(get_indent_value() << "---------------tag type = " << code << ", tag length = " << length << " (" << get_tag_as_string(tag_code) << ")");
			switch(tag_code) {
			case Tag::END: finished = true; break;
			case Tag::SHOW_FRAME:                        ProcessShowFrame(obj, length); break;
			case Tag::DEFINE_SHAPE:                      ProcessDefineShape(obj, length); break;
			case Tag::PLACE_OBJECT:                      ProcessPlaceObject(obj, length); break;
			case Tag::REMOVE_OBJECT:                     ProcessRemoveObject(obj, length); break;
			case Tag::DEFINE_BITS:                       ProcessDefineBits(obj, length); break;
			case Tag::DEFINE_BUTTON:                     ProcessDefineButton(obj, length); break;
			case Tag::JPEG_TABLES:                       ProcessJPEGTables(obj, length); break;
			case Tag::SET_BACKGROUND_COLOR:              ProcessSetBackgroundColor(obj, length); break;
			case Tag::DEFINE_FONT:                       ProcessDefineFont(obj, length); break;
			case Tag::DEFINE_TEXT:                       ProcessDefineText(obj, length); break;
			case Tag::DO_ACTION:                         ProcessDoAction(obj, length); break;
			case Tag::DEFINE_FONT_INFO:                  ProcessDefineFontInfo(obj, length); break;
			case Tag::DEFINE_SOUND:                      ProcessDefineSound(obj, length); break;
			case Tag::START_SOUND:                       ProcessStartSound(obj, length); break;
			case Tag::DEFINE_BUTTON_SOUND:               ProcessDefineButtonSound(obj, length); break;
			case Tag::SOUND_STREAM_HEAD:                 ProcessSoundStreamHead(obj, length); break;
			case Tag::SOUND_STREAM_BLOCK:                ProcessSoundStreamBlock(obj, length); break;
			case Tag::DEFINE_BITS_LOSSLESS:              ProcessBitsLossless(obj, length); break;
			case Tag::DEFINE_BITS_JPEG2:                 ProcessDefineBitsJPEG2(obj, length); break;
			case Tag::DEFINE_SHAPE2:                     ProcessDefineShape2(obj, length); break;
			case Tag::DEFINE_BUTTON_CXFORM:              ProcessDefineButtonCxform(obj, length); break;
			case Tag::PROTECT:                           ProcessProtect(obj, length); break;
			case Tag::PLACE_OBJECT2:                     ProcessPlaceObject2(obj, length); break;
			case Tag::REMOVE_OBJECT2:                    ProcessRemoveObject2(obj, length); break;
			case Tag::DEFINE_SHAPE3:                     ProcessDefineShape3(obj, length); break;
			case Tag::DEFINE_TEXT2:                      ProcessDefineText2(obj, length); break;
			case Tag::DEFINE_BUTTON2:                    ProcessDefineButton2(obj, length); break;
			case Tag::DEFINE_BITS_JPEG3:                 ProcessDefineBitsJPEG3(obj, length); break;
			case Tag::DEFINE_BITS_LOSSLESS2:             ProcessDefineBitsLossless2(obj, length); break;
			case Tag::DEFINE_EDIT_TEXT:                  ProcessDefineEditText(obj, length); break;
			case Tag::DEFINE_SPRITE:                     ProcessDefineSprite(obj, length); break;
			case Tag::FRAME_LABEL:                       ProcessFrameLabel(obj, length); break;
			case Tag::SOUND_STREAM_HEAD2:                ProcessSoundStreamHead2(obj, length); break;
			case Tag::DEFINE_MORPH_SHAPE:                ProcessDefineMorphShape(obj, length); break;
			case Tag::DEFINE_FONT2:                      ProcessDefineFont2(obj, length); break;
			case Tag::EXPORT_ASSETS:                     ProcessExportAssets(obj, length); break;
			case Tag::IMPORT_ASSETS:                     ProcessImportAssets(obj, length); break;
			case Tag::ENABLE_DEBUGGER:                   ProcessEnableDebugger(obj, length); break;
			case Tag::DO_INIT_ACTION:                    ProcessDoInitAction(obj, length); break;
			case Tag::DEFINE_VIDEO_STREAM:               ProcessDefineVideoStream(obj, length); break;
			case Tag::DEFINE_FONT_INFO2:                 ProcessDefineFontInfo2(obj, length); break;
			case Tag::ENABLE_DEBUGGER2:                  ProcessEnableDebugger2(obj, length); break;
			case Tag::SCRIPT_LIMITS:                     ProcessScriptLimits(obj, length); break;
			case Tag::SET_TAB_INDEX:                     ProcessSetTabIndex(obj, length); break;
			case Tag::FILE_ATTRIBUTES:                   ProcessFileAttributes(obj, length); break;
			case Tag::PLACE_OBJECT3:                     ProcessPlaceObject3(obj, length); break;
			case Tag::IMPORT_ASSETS2:                    ProcessImportAssets2(obj, length); break;
			case Tag::DEFINE_FONT_ALIGN_ZONES:           ProcessDefineFontAlignZones(obj, length); break;
			case Tag::DEFINE_CSM_TEXT_SETTINGS:          ProcessDefineCSMTextSettings(obj, length); break;
			case Tag::DEFINE_FONT3:                      ProcessDefineFont3(obj, length); break;
			case Tag::SYMBOL_CLASS:                      ProcessSymbolClass(obj, length); break;
			case Tag::METADATA:                          ProcessMetadata(obj, length); break;
			case Tag::DEFINE_SCALING_GRID:               ProcessDefineScalingGrid(obj, length); break;
			case Tag::DO_ABC:                            ProcessDoABC(obj, length); break;
			case Tag::DEFINE_SHAPE4:                     ProcessDefineShape4(obj, length); break;
			case Tag::DEFINE_MORPH_SHAPE2:               ProcessDefineMorphShape2(obj, length); break;
			case Tag::DEFINE_SCENE_AND_FRAME_LABEL_DATA: ProcessDefineSceneAndFrameLabelData(obj, length); break;
			case Tag::DEFINE_BINARY_DATA:                ProcessDefineBinaryData(obj, length); break;
			case Tag::DEFINE_FONT_NAME:                  ProcessDefineFontName(obj, length); break;
			case Tag::START_SOUND2:                      ProcessStartSound2(obj, length); break;
			case Tag::DEFINE_JPEG_BITS4:                 ProcessDefineJPEGBits4(obj, length); break;
			case Tag::DEFINE_FONT4:                      ProcessDefineFont4(obj, length); break;
			case Tag::ENABLE_TELEMETRY:                  ProcessEnableTelemetry(obj, length); break;
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

	void reader::ProcessShowFrame(movie& obj, unsigned length)
	{
		//obj.draw();
	}


	void reader::ProcessDefineShape(movie& obj, unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(1, bits_);
		obj.add_character(s->id(), s);
		LOG_DEBUG("\tAdded shape with ID(" << s->id() << ")");
	}


	void reader::ProcessPlaceObject(movie& obj, unsigned length)
	{
		eat_bit_stream(length);
		std::cerr << "Ignored PlaceObject tag" << std::endl;
	}


	void reader::ProcessRemoveObject(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'RemoveObject'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineBits(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineBits'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButton(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButton'");
		eat_bit_stream(length);
	}


	void reader::ProcessJPEGTables(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'JPEGTables'");
		eat_bit_stream(length);
	}


	void reader::ProcessSetBackgroundColor(movie& obj, unsigned length)
	{
		obj.set_background_color(bits_->read_rgb());
		LOG_DEBUG("\tbackground color: " << obj.get_background_color());
	}


	void reader::ProcessDefineFont(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineText(movie& obj, unsigned length)
	{
		text* t = new text;
		t->read(bits_);
		// Add text to character list.
		obj.add_character(t->id(), t);
	}


	void reader::ProcessDoAction(movie& obj, unsigned length)
	{
		if(obj.use_as3() && obj.version() >= 9) {
			std::cerr << "Ignoring DoAction tag -- use_as3 bit set.\n";
			eat_bit_stream(length);
		} else {
			
			eat_bit_stream(length);
		}
	}


	void reader::ProcessDefineFontInfo(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFontInfo'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSound(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessStartSound(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'StartSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButtonSound(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButtonSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamHead(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamHead'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamBlock(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamBlock'");
		eat_bit_stream(length);
	}


	void reader::ProcessBitsLossless(movie& obj, unsigned length)
	{
		image_character* img = new image_character(1, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read_lossless(bits_);
		obj.add_character(img->id(), img);
	}


	void reader::ProcessDefineBitsJPEG2(movie& obj, unsigned length)
	{
		image_character* img = new image_character(2, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read(bits_);
		obj.add_character(img->id(), img);
	}


	void reader::ProcessDefineShape2(movie& obj, unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(2, bits_);
		obj.add_character(s->id(), s);
		LOG_DEBUG(get_indent_value() << "shape id: " << s->id());
	}


	void reader::ProcessDefineButtonCxform(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButtonCxform'");
		eat_bit_stream(length);
	}


	void reader::ProcessProtect(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'Protect'");
		eat_bit_stream(length);
	}


	void reader::ProcessPlaceObject2(movie& obj, unsigned length)
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
			display_instance = std::make_shared<displayable>(obj.find_character(bits_->read_unsigned16())));
		} else {
			display_instance = obj.find_displayable(depth);
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
				obj.remove_character_from_display_list(depth);
			}
			obj.add_character_to_display_list(depth, display_instance);
		}
	}


	void reader::ProcessRemoveObject2(movie& obj, unsigned length)
	{
		uint16_t depth = bits_->read_unsigned16();
		obj.remove_character_from_display_list(depth);
	}


	void reader::ProcessDefineShape3(movie& obj, unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(3, bits_);
		obj.add_character(s->id(), s);
		LOG_DEBUG(get_indent_value() << "shape ID(" << s->id() << ")");
	}


	void reader::ProcessDefineText2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineText2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButton2(movie& obj, unsigned length)
	{
		button* b = new button;
		b->set_id(bits_->read_unsigned16());
		b->read2(bits_);
		// Add text to character list.
		obj.add_character(b->id(), b);		
	}


	void reader::ProcessDefineBitsJPEG3(movie& obj, unsigned length)
	{
		image_character* img = new image_character(3, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read(bits_);
		obj.add_character(img->id(), img);
	}


	void reader::ProcessDefineBitsLossless2(movie& obj, unsigned length)
	{
		image_character* img = new image_character(2, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read_lossless(bits_);
		obj.add_character(img->id(), img);
	}


	void reader::ProcessDefineEditText(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineEditText'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSprite(movie& obj, unsigned length)
	{
		uint16_t sprite_id = bits_->read_unsigned16();
		uint16_t sprite_frame_count = bits_->read_unsigned16();
		LOG_DEBUG(get_indent_value() << "sprite id: " << sprite_id);
		LOG_DEBUG(get_indent_value() << "frame count: " << sprite_frame_count);
		push_indent(get_indent() + 2);
		read_tags(obj/* XXX this should be the sprite!*/);
		pop_indent();
	}


	void reader::ProcessFrameLabel(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'FrameLabel'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamHead2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamHead2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineMorphShape(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineMorphShape'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFont2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont2'");
		eat_bit_stream(length);
	}


	void reader::ProcessExportAssets(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ExportAssets'");
		eat_bit_stream(length);
	}


	void reader::ProcessImportAssets(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ImportAssets'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableDebugger(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'EnableDebugger'");
		eat_bit_stream(length);
	}


	void reader::ProcessDoInitAction(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DoInitAction'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineVideoStream(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineVideoStream'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontInfo2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFontInfo2'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableDebugger2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'EnableDebugger2'");
		eat_bit_stream(length);
	}


	void reader::ProcessScriptLimits(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ScriptLimits'");
		eat_bit_stream(length);
	}


	void reader::ProcessSetTabIndex(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SetTabIndex'");
		eat_bit_stream(length);
	}


	void reader::ProcessFileAttributes(movie& obj, unsigned length)
	{
		bits_->read_unsigned_bits(1); // reserved
		bool use_direct_blit = bits_->read_unsigned_bits(1) ? true : false;
		bool use_gpu = bits_->read_unsigned_bits(1) ? true : false;
		bool has_metadata = bits_->read_unsigned_bits(1) ? true : false;
		bool use_as3 = bits_->read_unsigned_bits(1) ? true : false;
		bits_->read_unsigned_bits(2); // reserved
		bool use_network = bits_->read_unsigned_bits(1) ? true : false;
		bits_->read_unsigned_bits(24); // reserved
		obj.set_attributes(use_direct_blit, use_gpu, use_as3, use_network);
	}


	void reader::ProcessPlaceObject3(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'PlaceObject3'");
		eat_bit_stream(length);
	}


	void reader::ProcessImportAssets2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ImportAssets2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontAlignZones(movie& obj, unsigned length)
	{
		uint16_t font_id = bits_->read_unsigned16();
		unsigned csm_table_hint = bits_->read_unsigned_bits(2);
		bits_->read_unsigned_bits(6); // reserved

		font_ptr f = boost::dynamic_pointer_cast<font>(obj.find_character(font_id));
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


	void reader::ProcessDefineCSMTextSettings(movie& obj, unsigned length)
	{
		uint16_t text_id = bits_->read_unsigned16();
		text_ptr t = boost::dynamic_pointer_cast<text>(obj.find_character(text_id));
		ASSERT_LOG(t != NULL, "Couldn't convert character with id: " << text_id << " to a text entity");
		t->set_render_type(bits_->read_unsigned_bits(2));
		t->set_grid_fit(bits_->read_unsigned_bits(3));
		bits_->read_unsigned_bits(3); // reserved
		t->set_thickness(bits_->read_float());
		t->set_sharpness(bits_->read_float());
		bits_->read_unsigned8(); // reserved
	}


	void reader::ProcessDefineFont3(movie& obj, unsigned length)
	{
		font* f = new font;
		f->read3(bits_);
		// Add front to character list.
		obj.add_character(f->id(), f);
	}


	void reader::ProcessSymbolClass(movie& obj, unsigned length)
	{
		uint32_t num_syms = uint32_t(bits_->read_unsigned16());
		for(uint32_t n = 0; n != num_syms; ++n) {
			uint16_t tag = bits_->read_unsigned16();
			std::string name = bits_->read_string();
			obj.add_symbol_class(tag, name);
		}
	}


	void reader::ProcessMetadata(movie& obj, unsigned length)
	{
		// deliberately ignore the metadata tag.
		std::cerr << "Ignoring metadata tag" << std::endl;
		eat_bit_stream(length);
	}


	void reader::ProcessDefineScalingGrid(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineScalingGrid'");
		eat_bit_stream(length);
	}


	void reader::ProcessDoABC(movie& obj, unsigned length)
	{
		uint32_t flags = bits_->read_unsigned32();
		std::string name = bits_->read_string();
		obj.add_abc(std::shared_ptr<avm2::abc_file>(new avm2::abc_file(flags, name, bits_)));
	}


	void reader::ProcessDefineShape4(movie& obj, unsigned length)
	{
		shape* s = new shape;
		s->set_id(bits_->read_unsigned16());
		s->read(4, bits_);
		obj.add_character(s->id(), s);
		LOG_DEBUG("\tAdded shape with ID(" << s->id() << ")");
	}


	void reader::ProcessDefineMorphShape2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineMorphShape2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSceneAndFrameLabelData(movie& obj, unsigned length)
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

		obj.set_scene_and_frame_info(scene, frame_label);
	}


	void reader::ProcessDefineBinaryData(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineBinaryData'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontName(movie& obj, unsigned length)
	{
		std::cerr << "Ignoring DefineFontName tag" << std::endl;
		eat_bit_stream(length);
	}


	void reader::ProcessStartSound2(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'StartSound2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineJPEGBits4(movie& obj, unsigned length)
	{
		image_character* img = new image_character(4, length-2);
		img->set_id(bits_->read_unsigned16());
		img->read(bits_);
		obj.add_character(img->id(), img);
	}


	void reader::ProcessDefineFont4(movie& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont4'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableTelemetry(movie& obj, unsigned length)
	{
		std::cerr << "Ignoring 'EnableTelemetry' tag" << std::endl;
		eat_bit_stream(length);
	}

}
