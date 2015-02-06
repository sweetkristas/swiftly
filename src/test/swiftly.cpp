#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../swf_player.hpp"
#include "profile_timer.hpp"

#include "kre/CameraObject.hpp"
#include "kre/SDLWrapper.hpp"
#include "kre/RenderManager.hpp"
#include "kre/RenderQueue.hpp"
#include "kre/SceneGraph.hpp"
#include "kre/SceneNode.hpp"
#include "kre/WindowManager.hpp"

// Approximate delay between frames.
#define FRAME_RATE	1000 / 60

bool process_events()
{
	SDL_Event e;
	while(SDL_PollEvent(&e)) {
		switch(e.type) {
		case SDL_MOUSEWHEEL:
			break;
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				return false;
			} else if(e.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
			} else if(e.key.keysym.scancode == SDL_SCANCODE_LEFT) {
			} else if(e.key.keysym.scancode == SDL_SCANCODE_DOWN) {
			} else if(e.key.keysym.scancode == SDL_SCANCODE_UP) {
			}
			break;
		}
	}

	return true;
}


int main(int argc, char* argv[]) 
{
	std::vector<std::string> args;
	for(int i = 0; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	int width = -1;
	int height = -1;
	for(auto it  = args.begin(); it != args.end(); ++it) {
		size_t sep = it->find('=');
		std::string arg_name = *it;
		std::string arg_value;
		if(sep != std::string::npos) {
			arg_name = it->substr(0,sep);
			arg_value = it->substr(sep+1);
		}

		if(arg_name == "--width") {
			width = boost::lexical_cast<int>(arg_value);
		} else if(arg_name == "--height") {
			height = boost::lexical_cast<int>(arg_value);
		}
	}

	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

	auto player = swf::player::create();
	player->load_file("data\\clip_as_button.swf");
	player->set_transparent_background();
	//swf::reader swf_reader("data\\test-menu.swf", swf_object);
	//swf::reader swf_reader("data\\swf-test2.swf", swf_object);
	//swf::reader swf_reader("data\\as3_test1.swf", swf_object);
	auto swf_root = player->get_root_movie();
	if(width == -1 || height == -1) {
		const rect& r = swf_root->get_frame_size();
		width = r.w()/swf_root->twip();
		height = r.h()/swf_root->twip();
	}

	try {

		using namespace KRE;

		SDL::SDL_ptr manager(new SDL::SDL());
		WindowManagerPtr main_wnd = WindowManager::createInstance("SDL", "opengl");
		main_wnd->enableVsync(false);
		main_wnd->createWindow(width, height);
		main_wnd->setWindowTitle("Swiftly");

		SceneGraphPtr scene = SceneGraph::create("main");
		SceneNodePtr root = scene->getRootNode();
		root->setNodeName("root_node");

		player->set_render_attach_fn(
			[&root](SceneObjectPtr o) { root->attachObject(o); },
			[&root](SceneObjectPtr o) { root->removeObject(o); });

		auto scenecam = Camera::createInstance("cam0", 0, width, 0, height);
		scenecam->createFrustum();
		root->attachCamera(scenecam);

		auto rman = RenderManager::getInstance();
		rman->addQueue(0, "opaques");


		bool running = true;
		Uint32 start_time = SDL_GetTicks();
		uint64_t render_acc = 0;
		int render_cnt = 0;

		profile::timer delta_time;
		while(running) {
			Uint32 cycle_start_tick = SDL_GetTicks();

			main_wnd->clear(ClearFlags::ALL);
			scene->process(SDL_GetTicks() / 1000.0f);

			running = process_events();

			swf_root->update(delta_time.get_time());
			delta_time.reset();
			swf_root->draw();

			scene->renderScene(rman);
			rman->render(main_wnd);

			main_wnd->swap();

			Uint32 delay = SDL_GetTicks() - cycle_start_tick;
			render_acc += delay;
			render_cnt++;
			Uint32 current_time = SDL_GetTicks();
			if(current_time - start_time >= 1000) {
				std::cerr << "Average processing time: " << double(render_acc)/double(render_cnt) << " ms" << std::endl;

				start_time = current_time;
				render_cnt = 0;
				render_acc = 0;
			}

			if(delay > FRAME_RATE) {
				std::cerr << "CYCLE TOO LONG: " << delay << std::endl;
			} else {
				SDL_Delay(FRAME_RATE - delay);
			}
		}



	} catch(std::exception& e) {
		std::cerr << e.what();
	}
	return 0;
}
