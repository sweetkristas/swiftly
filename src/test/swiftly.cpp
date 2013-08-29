#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sdl_wrapper.hpp"
#include "../swf.hpp"
#include "../swf_reader.hpp"
#include "wm.hpp"

// Approximate delay between frames.
#define FRAME_RATE	1000 / 60

void sdl_gl_setup()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
}

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

	swf::swf swf_object;
	swf_object.set_transparent_background();
	//swf::reader swf_reader("data\\test-menu.swf", swf_object);
	//swf::reader swf_reader("data\\swf-test2.swf", swf_object);
	swf::reader swf_reader("data\\as3_test1.swf", swf_object);
	if(width == -1 || height == -1) {
		const geometry::rect& r = swf_object.frame_size();
		width = (r.x2 - r.x1)/swf_object.twip();
		height = (r.y2 - r.y1)/swf_object.twip();
	}

	try {
		graphics::SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
		graphics::window_manager wm;
		sdl_gl_setup();
		wm.create_window("swiftly_test", 
			SDL_WINDOWPOS_CENTERED, 
			SDL_WINDOWPOS_CENTERED, 
			width, 
			height, 
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
		wm.set_icon("images/icon.png");
		wm.gl_init();

		bool running = true;
		Uint32 start_time = SDL_GetTicks();
		uint64_t render_acc = 0;
		int render_cnt = 0;

		glMatrixMode(GL_PROJECTION);
		glm::mat4 projmat = glm::ortho(0.0f, float(width), 0.0f, float(height));
		glLoadMatrixf(glm::value_ptr(projmat));
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		while(running) {
			Uint32 cycle_start_tick = SDL_GetTicks();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			running = process_events();

			swf_object.advance();

			wm.swap();

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
