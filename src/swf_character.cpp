#include <algorithm>
#include "swf_character.hpp"
#include "swf_player.hpp"

namespace swf
{
	character_def::character_def()
		: as_object_def()
	{
	}

	character_def::~character_def()
	{
	}

	void character_def::add_symbol_class(int character_id, const std::string& class_name)
	{
		as3_symbol_map_[character_id] = class_name;
	}

	character::character(const weak_player_ptr& player, const character_ptr& parent, int id, const character_def_ptr& def)
		: as_object(player),
		  parent_(parent),
		  id_(id),
		  def_(def),
		  background_color_()
	{
	}

	character::~character()
	{
	}

	void character::set_background_alpha(int a)
	{
		background_color_.a = std::min(255, std::max(0, a));
	}

	void character::set_background_color(const rgba& color)
	{
		background_color_ = color;
	}

	void character::add_to_display_list(const placement_params& pp)
	{
		auto it = display_list_.find(pp.get_depth());
		if(it != display_list_.end()) {
			LOG_WARN("Adding character at depth " << pp.get_depth() << " replaces existing character.");
		}
		ASSERT_LOG(pp.has_id(), "Adding character at depth: " << pp.get_depth() << "But has no id.");
		display_list_[pp.get_depth()] = get_player()->get_root_movie()->get_character_from_id(pp.get_id());
	}

	void character::remove_from_display_list(int depth)
	{
		auto it = display_list_.find(depth);
		ASSERT_LOG(it != display_list_.end(), "Removing item from display list at depth: " << depth << " and no item found.");
		display_list_.erase(it);
	}

	void character::modify_display_list(const placement_params& pp)
	{
		auto it = display_list_.find(pp.get_depth());
		ASSERT_LOG(it != display_list_.end(), "Modifying item on display list at depth: " << pp.get_depth() << " and no item found.");
		modify_character_params(it->second, pp);
	}

	void character::modify_character_params(const character_ptr& ch, const placement_params& pp)
	{
		if(pp.has_id()) {
			// ignore if we have an id, since modifying the character id would be instantiating a new character.
		}
		if(pp.has_matrix_transform()) {
		}
		if(pp.has_color_transform()) {
		}
		if(pp.has_morph_ratio()) {
		}
		if(pp.has_clip_depth()) { 
		}
		if(pp.has_name()) {
		}
		if(pp.has_clip_actions()) {
		}
	}

	character_ptr character_def::create_instance(const weak_player_ptr& player, const character_ptr& parent, int id)
	{
		return std::make_shared<character>(player, parent, id, shared_from_this());
	}
}
