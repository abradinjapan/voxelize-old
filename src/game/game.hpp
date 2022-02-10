#pragma once

#include "types.hpp"
#include "terrain.hpp"

namespace abradinjapan::voxelize {
    class game {
        user_input m_ui = user_input();
        SDL_Window* m_window = 0;
        SDL_GLContext m_context = 0;

        et initialize_libraries() {
            // initialize sdl2
            if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
                printf("Error: SDL2 could not be intialized!\n");
                fflush(stdout);
                return et::et_could_not_initialize_sdl2;
            }

            // create the window
            m_window = SDL_CreateWindow("Voxel Based Game! YEAH!", 0, 0, 720, 480, SDL_WINDOW_OPENGL);
            if (m_window == NULL) {
                printf("Error: SDL2 window could not be created!\n");
                fflush(stdout);
                return et::et_coult_not_create_sdl2_window;
            }

            // create the context
            m_context = SDL_GL_CreateContext(m_window);
            if (m_context == NULL) {
                printf("Error: SDL2 OpenGL context could not be created!\n");
                fflush(stdout);
                return et::et_could_not_create_sdl2_opengl_context;
            }

            // initilize glew
            if (glewInit() != GLEW_OK) {
                printf("Error: GLEW could not be initialized!\n");
                fflush(stdout);
                return et::et_could_not_initialize_glew;
            }

            return et::et_no_error;
        }

    public:
        et play() {
            // initialize error variable
            et error = et::et_no_error;

            // initialize libraries
            error = initialize_libraries();
            if (error != et::et_no_error) {
                return error;
            }

            // initialize variables
            shaders* s = new shaders();
            texture* t = new texture();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            chunk_888** cs = new chunk_888*[64];
            //chunk_side_88** css = new chunk_side_88*[(8 * 3) + 1]; // chunk sides
            float* vertices_buffer = new float[(3 + 2) * 6 * 6 * 512 * 3];
            float cam_move = 0.0f, cam_pitch = 0.0f, cam_yaw = 0.0f;
            //unsigned char* chunk_buffer = new unsigned char[64];

            // use shaders
            s->use_shaders((char*)"./src/shaders/v5/");
            if (s->p_error < 0) {
                return et::et_error_unknown;
            }

            // change opengl states
            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0, 0.0, 1.0, 1.0);
            
            // initialize vertices
            for (unsigned int i = 0; i < 8; i++) {
                for (unsigned int j = 0; j < 8; j++) {
                    cs[i + (j * 8)] = generate_chunk(0, 0, 0);
                    cs[i + (j * 8)]->initialize();
                    cs[i + (j * 8)]->send_to_gpu(vertices_buffer, (float)i - 8.0f, (float)j - 8.0f, 0.0f);
                }
            }

            // create texture
            t->initialize((char*)"./assets/textures/test.png", GL_TEXTURE_2D, &error);
            if (error != et::et_no_error) {
                return error; // TODO: Exit Cleanly
            }

            t->send_texture_to_gpu();

            // run game
            while (!m_ui.quit()) {
                // get input
                m_ui.update();

                // display screen
                // clear screen
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                // update camera
                // move textured box in 3d space
                cam_move = 0.0f;
                
                if (m_ui.w()) {
                    cam_move = 0.5f;
                    cam_pitch = 1.0f;
                }
                if (m_ui.s()) {
                    cam_move = 0.5f;
                    cam_pitch = -1.0f;
                }
                if (m_ui.a()) {
                    cam_move = 0.5f;
                    cam_yaw = 1.0f;
                }
                if (m_ui.d()) {
                    cam_move = 0.5f;
                    cam_yaw = -1.0f;
                }

                model = glm::rotate(model, glm::radians(cam_move), glm::vec3(cam_pitch, cam_yaw, 1.0f));
                view = glm::lookAt(glm::vec3(8.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //glm::lookAt(camera_position, camera_position + camera_front, camera_up);
                projection = glm::perspective(glm::radians(45.0f), 720.0f / 480.0f, 0.1f, 100.0f);
                
                glUniformMatrix4fv(glGetUniformLocation(s->p_shaders_program_ID, "u_model"), 1, GL_FALSE, glm::value_ptr(model));
                glUniformMatrix4fv(glGetUniformLocation(s->p_shaders_program_ID, "u_view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(s->p_shaders_program_ID, "u_projection"), 1, GL_FALSE, glm::value_ptr(projection));

                // do drawing
                // draw textured box
                t->bind();
                glUniform1i(glGetUniformLocation(s->p_shaders_program_ID, "u_texture_1"), 0);

                for (unsigned int i = 0; i < 64; i++) {
                    cs[i]->bind();
                    cs[i]->draw();
                    cs[i]->unbind();
                }

                /*for (unsigned int i = 0; i < 6; i++) {
                    css[i]->bind();
                    css[i]->draw();
                    css[i]->unbind();
                }*/

                t->unbind();

                // update window
                SDL_GL_SwapWindow(m_window);
            }

            /*for (unsigned int i = 0; i < 6; i++) {
                css[i]->uninitialize();
                delete css[i];
            }*/
            
            for (unsigned int i = 0; i < 64; i++) {
                delete cs[i];
            }
            delete[] cs;
            
            t->uninitialize();

            delete[] vertices_buffer;
            //delete css;
            delete t;
            delete s;

            SDL_GL_DeleteContext(m_context);
            SDL_DestroyWindow(m_window);
            SDL_Quit();

            return error;
        }
    };
}