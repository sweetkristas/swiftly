#include <map>
#include <sstream>
#include "asserts.hpp"
#include "avm2.hpp"
#include "decompress.hpp"
#include "swf_character.hpp"
#include "swf_command.hpp"
#include "swf_button.hpp"
#include "swf_image.hpp"
#include "swf_font.hpp"
#include "swf_movie.hpp"
#include "swf_player.hpp"
#include "swf_reader.hpp"
#include "swf_shape.hpp"
#include "swf_sprite.hpp"
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

	reader::reader(const std::string& fname, const player_ptr& play) 
		: player_(play)
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

		rect frame_size = bits_->read_rect();
		fixed_point frame_rate = bits_->read_fixedpoint_bits(16);
		uint16_t frame_count = bits_->read_unsigned16();

		std::cerr << "Frame Size: " << frame_size.x1()/20.0 << "," << frame_size.y1()/20.0 << "," << frame_size.x2()/20.0 << "," << frame_size.y2()/20.0 << std::endl;
		std::cerr << "Frame Rate: " << frame_rate.to_double() << ", Frame Count: " << frame_count << std::endl;

		player_->set_version(swf_version);
		auto root = player_->get_root_movie();
		ASSERT_LOG(root != nullptr, "No root movie defined.");
		root->set_frame_size(frame_size);
		root->set_frame_rate(frame_rate.to_float());
		root->set_frame_count(frame_count);

		read_tags(root->get_movie_def());
	}

	void reader::read_tags(const character_def_ptr& obj)
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
			LOG_DEBUG("---------------tag type = " << code << ", tag length = " << length << " (" << get_tag_as_string(tag_code) << ")");
			if(!obj->is_tag_allowable(tag_code)) {
				LOG_ERROR("Tag: " << get_tag_as_string(tag_code) << " isn't allowable as part of definiton. Skipping.");
				eat_bit_stream(length);
				continue;
			}
			int start_pos = bits_->get_bits_read();
			switch(tag_code) {
			case Tag::END: finished = true; break;
			case Tag::SHOW_FRAME:                        ProcessShowFrame(obj, length); break;
			case Tag::DEFINE_SHAPE:                      ProcessDefineShape(obj, length); break;
			case Tag::PLACE_OBJECT:                      ProcessPlaceObject(obj, length); break;
			case Tag::REMOVE_OBJECT:                     ProcessRemoveObject(obj, length); break;
			case Tag::DEFINE_BITS:                       ProcessDefineBits(obj, length); break;
			case Tag::DEFINE_BUTTON:                     ProcessDefineButton(obj, length); break;
			case Tag::JPEG_TABLES:                       ProcessJPEGTables(obj, length); break;
			case Tag::SET_BACKGROUND_COLOR_RGB:          ProcessSetBackgroundColor(obj, length); break;
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

			int bytes_read = (bits_->get_bits_read() - start_pos)/8;
			ASSERT_LOG(bytes_read == length, "Error in tag: " << get_tag_as_string(tag_code) << " bytes_read: " << bytes_read << " != length: " << length);
		}
	}

	void reader::eat_bit_stream(unsigned length)
	{
		// For tags not processed call this function to remove data from the bit stream
		while(length--) {
			bits_->read_unsigned8();
		}
	}

	void reader::ProcessShowFrame(const character_def_ptr& obj, unsigned length)
	{
		obj->show_frame();
	}


	void reader::ProcessDefineShape(const character_def_ptr& obj, unsigned length)
	{
		auto s = shape_def::create();
		auto id = bits_->read_unsigned16();
		s->read(1, bits_);
		obj->add_character(id, s);
		LOG_DEBUG("define shape id: " << id);
	}


	void reader::ProcessPlaceObject(const character_def_ptr& obj, unsigned length)
	{
		int bits_read = bits_->get_bits_read();
		uint16_t id = bits_->read_unsigned16();
		uint16_t depth = bits_->read_unsigned16();
		matrix2x3 trf = bits_->read_matrix();
		bool has_color_transform = false;
		color_transform ctrf;
		bits_read = (bits_->get_bits_read() - bits_read) / 8;
		if(static_cast<int>(length) > bits_read) {
			ctrf = bits_->read_cxform();
			has_color_transform = true;
		}
		auto placeobj = place_object::create(id, depth, trf, has_color_transform, ctrf);
		obj->add_command(placeobj);
	}


	void reader::ProcessRemoveObject(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'RemoveObject'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineBits(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineBits'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButton(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButton'");
		eat_bit_stream(length);
	}


	void reader::ProcessJPEGTables(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'JPEGTables'");
		eat_bit_stream(length);
	}


	void reader::ProcessSetBackgroundColor(const character_def_ptr& obj, unsigned length)
	{
		auto bkgcolor = set_background_color::create(bits_->read_rgb());
		obj->add_command(bkgcolor);
	}


	void reader::ProcessDefineFont(const character_def_ptr& obj, unsigned length)
	{
		auto f = font_def::create();
		int id = bits_->read_unsigned16();
		f->read(1, bits_);
		obj->add_character(id, f);
	}


	void reader::ProcessDefineText(const character_def_ptr& obj, unsigned length)
	{
		auto t = text_def::create();
		int id = bits_->read_unsigned16();
		t->read(bits_);
		obj->add_character(id, t);
	}


	void reader::ProcessDoAction(const character_def_ptr& obj, unsigned length)
	{
		if(player_->use_as3() && bits_->swf_version() >= 9) {
			std::cerr << "Ignoring DoAction tag -- use_as3 bit set.\n";
			eat_bit_stream(length);
		} else {
			auto ap = action::create(bits_);
			auto act = do_action::create(ap);
			
			obj->add_command(act);
		}
	}


	void reader::ProcessDefineFontInfo(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFontInfo'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSound(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessStartSound(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'StartSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButtonSound(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButtonSound'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamHead(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamHead'");
		eat_bit_stream(length);
	}


	void reader::ProcessSoundStreamBlock(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamBlock'");
		eat_bit_stream(length);
	}


	void reader::ProcessBitsLossless(const character_def_ptr& obj, unsigned length)
	{
		auto img = image_character_def::create(1, length-2);
		int id = bits_->read_unsigned16();
		img->read_lossless(bits_);
		obj->add_character(id, img);
	}


	void reader::ProcessDefineBitsJPEG2(const character_def_ptr& obj, unsigned length)
	{
		auto img = image_character_def::create(2, length-2);
		int id = bits_->read_unsigned16();
		img->read(bits_);
		obj->add_character(id, img);
	}


	void reader::ProcessDefineShape2(const character_def_ptr& obj, unsigned length)
	{
		auto s = shape_def::create();
		auto id = bits_->read_unsigned16();
		s->read(2, bits_);
		obj->add_character(id, s);
		LOG_DEBUG("define shape2 id: " << id);
	}


	void reader::ProcessDefineButtonCxform(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineButtonCxform'");
		eat_bit_stream(length);
	}


	void reader::ProcessProtect(const character_def_ptr& obj, unsigned length)
	{
		LOG_DEBUG("'Protect' tag ignored for player.");
		eat_bit_stream(length);
	}


	void reader::ProcessPlaceObject2(const character_def_ptr& obj, unsigned length)
	{
		bool has_clip_actions = bits_->read_unsigned_bits(1) ? true : false;
		bool has_clip_depth = bits_->read_unsigned_bits(1) ? true : false;
		bool has_name = bits_->read_unsigned_bits(1) ? true : false;
		bool has_ratio = bits_->read_unsigned_bits(1) ? true : false;
		bool has_color_transform = bits_->read_unsigned_bits(1) ? true : false;
		bool has_matrix = bits_->read_unsigned_bits(1) ? true : false;
		bool has_character = bits_->read_unsigned_bits(1) ? true : false;
		bool has_move = bits_->read_unsigned_bits(1) ? true : false;
		uint16_t depth = bits_->read_unsigned16();

		placement_params pp(depth);
		place_object2::Operation op = place_object2::Operation::MOVE;

		if(has_character) {
			pp.set_id(bits_->read_unsigned16());
			if(has_move) {
				op = place_object2::Operation::REPLACE;
			} else {
				op = place_object2::Operation::PLACE;
			}
		} else {
			if(has_move) {
				op = place_object2::Operation::MOVE;
			} else {
				// case isn't well defined.
				ASSERT_LOG(false, "XXX unhandled case of has_character=0 and has_move=0");
			}
		}

		if(has_matrix) {
			pp.set_matrix_transform(bits_->read_matrix());
		}
		if(has_color_transform) {
			pp.set_color_transform(bits_->read_cxform_with_alpha());
		}
		if(has_ratio) {
			pp.set_morph_ratio(bits_->read_unsigned16());
		}
		if(has_name) {
			pp.set_name(bits_->read_string());
		}
		if(has_clip_depth) {
			pp.set_clip_depth(bits_->read_unsigned16());
		}
		if(has_clip_actions) {
			pp.set_clip_actions(bits_->read_clip_actions());
		}

		auto placeobj = place_object2::create(op, pp);
		obj->add_command(placeobj);
	}


	void reader::ProcessRemoveObject2(const character_def_ptr& obj, unsigned length)
	{
		uint16_t depth = bits_->read_unsigned16();
		auto ro = remove_object2::create(depth);
		obj->add_command(ro);
	}


	void reader::ProcessDefineShape3(const character_def_ptr& obj, unsigned length)
	{
		auto s = shape_def::create();
		auto id = bits_->read_unsigned16();
		s->read(3, bits_);
		obj->add_character(id, s);
		LOG_DEBUG("define shape3 id: " << id);
	}


	void reader::ProcessDefineText2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineText2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineButton2(const character_def_ptr& obj, unsigned length)
	{
		auto b = button_def::create();
		int id = bits_->read_unsigned16();
		b->read2(bits_);
		// Add button to character list.
		obj->add_character(id, b);
	}


	void reader::ProcessDefineBitsJPEG3(const character_def_ptr& obj, unsigned length)
	{
		auto img = image_character_def::create(3, length-2);
		int id = bits_->read_unsigned16();
		img->read(bits_);
		obj->add_character(id, img);
	}


	void reader::ProcessDefineBitsLossless2(const character_def_ptr& obj, unsigned length)
	{
		auto img = image_character_def::create(2, length-2);
		int id = bits_->read_unsigned16();
		img->read_lossless(bits_);
		obj->add_character(id, img);
	}


	void reader::ProcessDefineEditText(const character_def_ptr& obj, unsigned length)
	{
		auto txt = edit_text_def::create();
		int id = bits_->read_unsigned16();
		txt->read(bits_);
		obj->add_character(id, txt);
		LOG_DEBUG("edit_text: " << id);
	}


	void reader::ProcessDefineSprite(const character_def_ptr& obj, unsigned length)
	{
		uint16_t id = bits_->read_unsigned16();
		uint16_t frame_count = bits_->read_unsigned16();
		auto spr = sprite_def::create(id, frame_count);
		LOG_DEBUG("sprite definition begins, id: " << id << ", frame_count: " << frame_count);
		read_tags(spr);
		LOG_DEBUG("sprite definition ends");
	}


	void reader::ProcessFrameLabel(const character_def_ptr& obj, unsigned length)
	{
		const std::string label = bits_->read_string();
		if(bits_->swf_version() >= 6 && label.size()+1 < length) {
			int name_anchor = bits_->read_unsigned8();
		}
		obj->set_frame_label(label);
	}


	void reader::ProcessSoundStreamHead2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SoundStreamHead2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineMorphShape(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineMorphShape'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFont2(const character_def_ptr& obj, unsigned length)
	{
		auto f = font_def::create();
		int id = bits_->read_unsigned16();
		f->read(2, bits_);
		obj->add_character(id, f);
	}


	void reader::ProcessExportAssets(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ExportAssets'");
		eat_bit_stream(length);
	}


	void reader::ProcessImportAssets(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ImportAssets'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableDebugger(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'EnableDebugger'");
		eat_bit_stream(length);
	}


	void reader::ProcessDoInitAction(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DoInitAction'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineVideoStream(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineVideoStream'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontInfo2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFontInfo2'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableDebugger2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'EnableDebugger2'");
		eat_bit_stream(length);
	}


	void reader::ProcessScriptLimits(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ScriptLimits'");
		eat_bit_stream(length);
	}


	void reader::ProcessSetTabIndex(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'SetTabIndex'");
		eat_bit_stream(length);
	}


	void reader::ProcessFileAttributes(const character_def_ptr& obj, unsigned length)
	{
		bits_->read_unsigned_bits(1); // reserved
		bool use_direct_blit = bits_->read_unsigned_bits(1) ? true : false;
		bool use_gpu = bits_->read_unsigned_bits(1) ? true : false;
		bool has_metadata = bits_->read_unsigned_bits(1) ? true : false;
		bool use_as3 = bits_->read_unsigned_bits(1) ? true : false;
		bits_->read_unsigned_bits(2); // reserved
		bool use_network = bits_->read_unsigned_bits(1) ? true : false;
		bits_->read_unsigned_bits(24); // reserved
		player_->set_attributes(use_direct_blit, use_gpu, use_as3, use_network);
	}


	void reader::ProcessPlaceObject3(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'PlaceObject3'");
		eat_bit_stream(length);
	}


	void reader::ProcessImportAssets2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ImportAssets2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontAlignZones(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'ProcessDefineFontAlignZones'");
		eat_bit_stream(length);
		/*uint16_t font_id = bits_->read_unsigned16();
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
		f->set_zone_records(&zrs);*/
	}


	void reader::ProcessDefineCSMTextSettings(const character_def_ptr& obj, unsigned length)
	{
		/*
		uint16_t text_id = bits_->read_unsigned16();
		text_ptr t = boost::dynamic_pointer_cast<text>(obj.find_character(text_id));
		ASSERT_LOG(t != NULL, "Couldn't convert character with id: " << text_id << " to a text entity");
		t->set_render_type(bits_->read_unsigned_bits(2));
		t->set_grid_fit(bits_->read_unsigned_bits(3));
		bits_->read_unsigned_bits(3); // reserved
		t->set_thickness(bits_->read_float());
		t->set_sharpness(bits_->read_float());
		bits_->read_unsigned8(); // reserved
		*/
		ASSERT_LOG(false, "Unhandled tag 'DefineCSMTextSettings'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFont3(const character_def_ptr& obj, unsigned length)
	{
		auto f = font_def::create();
		int id = bits_->read_unsigned16();
		f->read(3, bits_);
		obj->add_character(id, f);
	}


	void reader::ProcessSymbolClass(const character_def_ptr& obj, unsigned length)
	{
		uint32_t num_syms = uint32_t(bits_->read_unsigned16());
		for(uint32_t n = 0; n != num_syms; ++n) {
			uint16_t tag = bits_->read_unsigned16();
			std::string name = bits_->read_string();
			obj->add_symbol_class(tag, name);
		}
	}


	void reader::ProcessMetadata(const character_def_ptr& obj, unsigned length)
	{
		// deliberately ignore the metadata tag.
		LOG_INFO("Ignoring metadata tag");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineScalingGrid(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineScalingGrid'");
		eat_bit_stream(length);
	}


	void reader::ProcessDoABC(const character_def_ptr& obj, unsigned length)
	{
		/*uint32_t flags = bits_->read_unsigned32();
		std::string name = bits_->read_string();
		obj.add_abc(std::shared_ptr<avm2::abc_file>(new avm2::abc_file(flags, name, bits_)));
		*/
		ASSERT_LOG(false, "Unhandled tag 'DoABC'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineShape4(const character_def_ptr& obj, unsigned length)
	{
		auto s = shape_def::create();
		auto id = bits_->read_unsigned16();
		s->read(4, bits_);
		obj->add_character(id, s);
		LOG_DEBUG("define shape4 id: " << id);
	}


	void reader::ProcessDefineMorphShape2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineMorphShape2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineSceneAndFrameLabelData(const character_def_ptr& obj, unsigned length)
	{
		uint32_t num_scenes = bits_->read_unsigned32_encoded();
		for(int n = 0; n != num_scenes; ++n) {
			uint32_t scene_frame_offset = bits_->read_unsigned32_encoded();
			std::string scene_name = bits_->read_string();
			obj->add_scene_info(scene_frame_offset, scene_name);
		}

		uint32_t num_frame_labels = bits_->read_unsigned32_encoded();
		for(int n = 0; n != num_frame_labels; ++n) {
			uint32_t frame_num = bits_->read_unsigned32_encoded();
			std::string frame_label_str = bits_->read_string();
			obj->add_frame_label(frame_num, frame_label_str);
		}
	}


	void reader::ProcessDefineBinaryData(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineBinaryData'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineFontName(const character_def_ptr& obj, unsigned length)
	{
		std::cerr << "Ignoring DefineFontName tag" << std::endl;
		eat_bit_stream(length);
	}


	void reader::ProcessStartSound2(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'StartSound2'");
		eat_bit_stream(length);
	}


	void reader::ProcessDefineJPEGBits4(const character_def_ptr& obj, unsigned length)
	{
		auto img = image_character_def::create(4, length-2);
		int id = bits_->read_unsigned16();
		img->read(bits_);
		obj->add_character(id, img);
	}


	void reader::ProcessDefineFont4(const character_def_ptr& obj, unsigned length)
	{
		ASSERT_LOG(false, "Unhandled tag 'DefineFont4'");
		eat_bit_stream(length);
	}


	void reader::ProcessEnableTelemetry(const character_def_ptr& obj, unsigned length)
	{
		std::cerr << "Ignoring 'EnableTelemetry' tag" << std::endl;
		eat_bit_stream(length);
	}

}
