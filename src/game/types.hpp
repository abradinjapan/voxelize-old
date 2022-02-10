#pragma once

#include "lib.hpp"

#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <random>

namespace abradinjapan::voxelize {
    // error type
    enum et {
        // none
        et_no_error,

        // initializing libraries
        et_could_not_initialize_sdl2,
        et_coult_not_create_sdl2_window,
        et_could_not_create_sdl2_opengl_context,
        et_could_not_initialize_glew,
        
        // shaders

        // textures
        et_could_not_load_image,

        // other
        et_error_unknown
    };

    class shaders {
    public:
        GLuint p_shaders_program_ID = 0;
        char* p_vertex_shader_file_address = 0;
        char* p_fragment_shader_file_address = 0;
        char* p_vertex_shader_file = 0;
        char*p_fragment_shader_file = 0;
        GLuint p_vertex_shader_ID = 0;
        GLuint p_fragment_shader_ID = 0;
        long long p_error = 0;

        ~shaders() {
            glDeleteShader(p_vertex_shader_ID);
            glDeleteShader(p_fragment_shader_ID);
            delete[] p_vertex_shader_file_address;
            delete[] p_fragment_shader_file_address;
            delete[] p_vertex_shader_file;
            delete[] p_fragment_shader_file;
            glDeleteProgram(p_shaders_program_ID);
        }

    private:
        char* get_program_status(GLuint shader_ID, GLuint info_type) {
            int success;

            // get information
            glGetShaderiv(shader_ID, info_type, &success);
        
            // deal with information
            if (!success) {
                char* msg = new char[1024];

                // get msg
                glGetShaderInfoLog(shader_ID, 1024, NULL, msg);

                return msg;
            } else {
                return 0;
            }
        }
    
        char* compile_shader(const char* shader_source, GLuint shader_type) {
            GLuint shader_ID;
        
            // create shader
            if (shader_type == GL_VERTEX_SHADER) {
                p_vertex_shader_ID = glCreateShader(shader_type);
                shader_ID = p_vertex_shader_ID;
            } else if (shader_type == GL_FRAGMENT_SHADER) {
                p_fragment_shader_ID = glCreateShader(shader_type);
                shader_ID = p_fragment_shader_ID;
            }

            // transfer shader source to gpu
            glShaderSource(shader_ID, 1, (const GLchar* const*)&shader_source, NULL);

            // compile shader
            glCompileShader(shader_ID);

            // return status of compilation
            return get_program_status(shader_ID, GL_COMPILE_STATUS);
        }
    
        char* link_shaders() {
            p_shaders_program_ID = glCreateProgram();
        
            glAttachShader(p_shaders_program_ID, p_vertex_shader_ID);
            glAttachShader(p_shaders_program_ID, p_fragment_shader_ID);

            glLinkProgram(p_shaders_program_ID);

            int success;

            // get information
            glGetProgramiv(p_shaders_program_ID, GL_LINK_STATUS, &success);
        
            // deal with information
            if (!success) {
                char* msg = new char[1024];

                // get msg
                glGetProgramInfoLog(p_shaders_program_ID, 1024, NULL, msg);

                return msg;
            } else {
                return 0;
            }
        }

        long long compile_shaders() {
            char* error = 0;

            // load each file
            p_vertex_shader_file = load_file(p_vertex_shader_file_address);
            p_fragment_shader_file = load_file(p_fragment_shader_file_address);

            // compile each shader
            if ((error = compile_shader(p_vertex_shader_file, GL_VERTEX_SHADER)) != 0) {
                printf("Error: Vertex shader could not compile!\n%s", error);
            
                delete[] error;
                return -1;
            }

            if ((error = compile_shader(p_fragment_shader_file, GL_FRAGMENT_SHADER)) != 0) {
                printf("Error: Fragment shader could not compile!\n%s", error);
            
                delete[] error;
                return -2;
            }

            // assemble final program on gpu
            if ((error = link_shaders()) != 0) {
                printf("Error: Shaders could not be linked!\n%s", error);

                delete[] error;
                return -3;
            }

            // use final program
            glUseProgram(p_shaders_program_ID);

            return 0;
        }

    public:
        void use_shaders(char* folder_address) {
            p_vertex_shader_file_address = concatenate(folder_address, (char*)"vertex.glsl");
            p_fragment_shader_file_address = concatenate(folder_address, (char*)"fragment.glsl");
            p_error = compile_shaders();
        }
    };

    class user_input {
        // TODO: OPTIMIZE!
        bool m_quit_signal = false;
        bool m_w = false;
        bool m_s = false;
        bool m_a = false;
        bool m_d = false;

    public:
        void update() {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
                    m_quit_signal = true;
                }
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_w) {
                        m_w = true;
                    }
                    if (e.key.keysym.sym == SDLK_s) {
                        m_s = true;
                    }
                    if (e.key.keysym.sym == SDLK_a) {
                        m_a = true;
                    }
                    if (e.key.keysym.sym == SDLK_d) {
                        m_d = true;
                    }
                }
                if (e.type == SDL_KEYUP) {
                    if (e.key.keysym.sym == SDLK_w) {
                        m_w = false;
                    }
                    if (e.key.keysym.sym == SDLK_s) {
                        m_s = false;
                    }
                    if (e.key.keysym.sym == SDLK_a) {
                        m_a = false;
                    }
                    if (e.key.keysym.sym == SDLK_d) {
                        m_d = false;
                    }
                }
            }
        }

        bool quit() {
            return m_quit_signal;
        }

        bool w() {
            return m_w;
        }

        bool s() {
            return m_s;
        }

        bool a() {
            return m_a;
        }

        bool d() {
            return m_d;
        }
    };

    class texture {
        GLuint p_texture_ID = 0;
        GLenum p_texture_type = 0;
        int p_width = 0;
        int p_height = 0;
        int p_nrChannels = 0;
        unsigned char* p_texture_data = 0;

    public:
        void initialize(char* image_file_address, GLenum texture_type, et* error) {
            p_texture_type = texture_type;
            
            //stbi_set_flip_vertically_on_load(true);

            p_texture_data = stbi_load(image_file_address, &p_width, &p_height, &p_nrChannels, 0);
            if (p_texture_data == 0) {
                *error = et::et_could_not_load_image;
            }

            glGenTextures(1, &p_texture_ID);
        }

        void send_texture_to_gpu() {
            glActiveTexture(GL_TEXTURE0);
            bind();
            glTexParameteri(p_texture_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(p_texture_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(p_texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(p_texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(p_texture_type, 0, GL_RGBA, p_width, p_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, p_texture_data);
            glGenerateMipmap(p_texture_type);
            unbind();
        }

        void bind() {
            glBindTexture(p_texture_type, p_texture_ID);
        }

        void unbind() {
            glBindTexture(p_texture_type, 0);
        }

        void uninitialize() {
            glDeleteTextures(1, &p_texture_ID);
            stbi_image_free(p_texture_data);
        }
    };

    // surface type 2
    enum st2 {
        st2_front,
        st2_bottom,
        st2_left,
        st2_back,
        st2_top,
        st2_right
    };

    // texture vertex type
    enum tvt {
        tvt_bottom_left,
        tvt_bottom_right,
        tvt_top_left,
        tvt_top_right
    };

    // cube vertex type
    enum cvt {
        cvt_bottom_left_front,
        cvt_bottom_right_front,
        cvt_top_right_front,
        cvt_top_left_front,
        cvt_bottom_left_back,
        cvt_bottom_right_back,
        cvt_top_right_back,
        cvt_top_left_back
    };

    class chunk_888 {
        const unsigned short m_side_length = 8;
        const unsigned short m_block_count = 512;
        unsigned short m_blocks[512];
        GLuint m_vao, m_vbo, m_ebo;
        unsigned long long m_vbo_length, m_ebo_length;
        float* m_vbo_data;
        unsigned int* m_ebo_data;

    public:
        chunk_888() {
            m_vao = 0;
            m_vbo = 0;
            m_ebo = 0;
            m_vbo_length = 0;
            m_ebo_length = 0;
            m_vbo_data = 0;
            m_ebo_data = 0;
        }

    private:
        void write_vertex(float* vertices, unsigned int index, float x, float y, float z, float side_length, tvt texture_coord) {
            vertices[index] = x;
            vertices[index + 1] = y;
            vertices[index + 2] = z;

            if (texture_coord == tvt::tvt_bottom_left) {
                vertices[index + 3] = 0.0f;
                vertices[index + 4] = 0.0f;
            } else if (texture_coord == tvt::tvt_bottom_right) {
                vertices[index + 3] = 1.0f;
                vertices[index + 4] = 0.0f;
            } else if (texture_coord == tvt::tvt_top_left) {
                vertices[index + 3] = 0.0f;
                vertices[index + 4] = 1.0f;
            } else if (texture_coord == tvt::tvt_top_right) {
                vertices[index + 3] = 1.0f;
                vertices[index + 4] = 1.0f;
            }
        }

        void write_vertex_on_cube(float* vertices, unsigned int* index, float x, float y, float z, float l, tvt texture_vertex_type, cvt cube_vertex_type) {
            if (cube_vertex_type == cvt::cvt_bottom_left_front) {
                write_vertex(vertices, *index, x, y, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_bottom_right_front) {
                write_vertex(vertices, *index, x + l, y, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_left_front) {
                write_vertex(vertices, *index, x, y + l, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_right_front) {
                write_vertex(vertices, *index, x + l, y + l, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_bottom_left_back) {
                write_vertex(vertices, *index, x, y, z - l, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_bottom_right_back) {
                write_vertex(vertices, *index, x + l, y, z - l, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_left_back) {
                write_vertex(vertices, *index, x, y + l, z - l, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_right_back) {
                write_vertex(vertices, *index, x + l, y + l, z - l, l, texture_vertex_type);
            }

            *index += 5;
        }

        void write_face(float* vertices, unsigned int* index, float x, float y, float z, float l, st2 surface_type) {
            cvt front[] = {
                cvt::cvt_bottom_left_front,
                cvt::cvt_bottom_right_front,
                cvt::cvt_top_left_front,
                cvt::cvt_top_right_front
            };

            cvt bottom[] = {
                cvt::cvt_bottom_left_front,
                cvt::cvt_bottom_right_front,
                cvt::cvt_bottom_left_back,
                cvt::cvt_bottom_right_back
            };

            cvt left[] = {
                cvt::cvt_bottom_left_front,
                cvt::cvt_bottom_left_back,
                cvt::cvt_top_left_front,
                cvt::cvt_top_left_back
            };

            cvt back[] = {
                cvt::cvt_bottom_left_back,
                cvt::cvt_bottom_right_back,
                cvt::cvt_top_left_back,
                cvt::cvt_top_right_back
            };

            cvt top[] = {
                cvt::cvt_top_left_front,
                cvt::cvt_top_right_front,
                cvt::cvt_top_left_back,
                cvt::cvt_top_right_back
            };

            cvt right[] = {
                cvt::cvt_bottom_right_front,
                cvt::cvt_bottom_right_back,
                cvt::cvt_top_right_front,
                cvt::cvt_top_right_back
            };
            
            switch (surface_type) {
            case st2::st2_front:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, front[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, front[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, front[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, front[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, front[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, front[2]);
                break;
            case st2::st2_bottom:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, bottom[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, bottom[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, bottom[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, bottom[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, bottom[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, bottom[2]);
                break;
            case st2::st2_left:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, left[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, left[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, left[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, left[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, left[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, left[2]);
                break;
            case st2::st2_back:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, back[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, back[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, back[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, back[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, back[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, back[2]);
                break;
            case st2::st2_top:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, top[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, top[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, top[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, top[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, top[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, top[2]);
                break;
            case st2::st2_right:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, right[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, right[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, right[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, right[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, right[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, right[2]);
                break;
            }
        }

        bool bounds_check_face(int x, int y, int z, st2 face) {
            const bool sides = false;

            if (face == st2::st2_front) {
                if (z % 8 == 7) {
                    return sides;
                }
                if (m_blocks[x + (y * 8) + ((z + 1) * 64)] > 0) {
                    return false;
                }
            }
            if (face == st2::st2_back) {
                if (z % 8 == 0) {
                    return sides;
                }
                if (m_blocks[x + (y * 8) + ((z - 1) * 64)] > 0) {
                    return false;
                }
            }
            if (face == st2::st2_top) {
                if (y % 8 == 7) {
                    return sides;
                }
                if (m_blocks[x + ((y + 1) * 8) + (z * 64)] > 0) {
                    return false;
                }
            }
            if (face == st2::st2_bottom) {
                if (y % 8 == 0) {
                    return sides;
                }
                if (m_blocks[x + ((y - 1) * 8) + (z * 64)] > 0) {
                    return false;
                }
            }
            if (face == st2::st2_right) {
                if (x % 8 == 7) {
                    return sides;
                }
                if (m_blocks[x + 1 + (y * 8) + (z * 64)] > 0) {
                    return false;
                }
            }
            if (face == st2::st2_left) {
                if (x % 8 == 0) {
                    return sides;
                }
                if (m_blocks[x - 1 + (y * 8) + (z * 64)] > 0) {
                    return false;
                }
            }

            return true;
        }

        void render_inside(float* points, float x_offset, float y_offset, float z_offset) {
            float side_length = 1.0f / 8.0f;
            unsigned int points_index = 0;
            unsigned int vbo_index = 0;
            unsigned int ebo_index = 0;

            // generate all points
            for (unsigned int x = 0; x < m_side_length; x++) {
                for (unsigned int y = 0; y < m_side_length; y++) {
                    for (unsigned int z = 0; z < m_side_length; z++) {
                        if (m_blocks[x + (y * 8) + (z * 64)] != 0) {
                            if (bounds_check_face(x, y, z, st2::st2_front)) {
                                write_face(points, &points_index, side_length * (float)x + x_offset, side_length * (float)y + y_offset, side_length * (float)z + z_offset, side_length, st2::st2_front);
                            }
                            if (bounds_check_face(x, y, z, st2::st2_bottom)) {
                                write_face(points, &points_index, side_length * (float)x + x_offset, side_length * (float)y + y_offset, side_length * (float)z + z_offset, side_length, st2::st2_bottom);
                            }
                            if (bounds_check_face(x, y, z, st2::st2_left)) {
                                write_face(points, &points_index, side_length * (float)x + x_offset, side_length * (float)y + y_offset, side_length * (float)z + z_offset, side_length, st2::st2_left);
                            }
                            if (bounds_check_face(x, y, z, st2::st2_back)) {
                                write_face(points, &points_index, side_length * (float)x + x_offset, side_length * (float)y + y_offset, side_length * (float)z + z_offset, side_length, st2::st2_back);
                            }
                            if (bounds_check_face(x, y, z, st2::st2_top)) {
                                write_face(points, &points_index, side_length * (float)x + x_offset, side_length * (float)y + y_offset, side_length * (float)z + z_offset, side_length, st2::st2_top);
                            }
                            if (bounds_check_face(x, y, z, st2::st2_right)) {
                                write_face(points, &points_index, side_length * (float)x + x_offset, side_length * (float)y + y_offset, side_length * (float)z + z_offset, side_length, st2::st2_right);
                            }
                        }
                    }
                }
            }

            // compress vbo and ebo data
            // TODO

            // write to m_vbo_data
            m_vbo_length = points_index;
            m_vbo_data = new float[m_vbo_length];

            for (unsigned int i = 0; i < m_vbo_length; i++) {
                m_vbo_data[i] = points[i];
            }

            // write ebo data
            m_ebo_length = points_index / 5;
            m_ebo_data = new unsigned int[m_ebo_length];

            for (unsigned int i = 0; i < m_ebo_length; i++) {
                m_ebo_data[i] = i;
            }
        }

    public:
        void set_chunk_data_as_air() {
            for (unsigned int i = 0; i < 512; i++) {
                m_blocks[i] = 0;
            }
        }

        void set_chunk_data_as_random() {
            std::random_device random_device;
            std::mt19937 random_number_generator(random_device());

            for (unsigned int i = 0; i < 512; i++) {
                m_blocks[i] = random_number_generator() % 2;
            }
        }

        void initialize() {
            // setup opengl buffers
            glGenVertexArrays(1, &m_vao);
            glGenBuffers(1, &m_vbo);
            glGenBuffers(1, &m_ebo);
        }

        void set_block_at(unsigned int x, unsigned int y, unsigned int z, unsigned short value) {
            m_blocks[x + (y * 8) + (z * 64)] = value;
        }

        unsigned short get_block_at(unsigned int x, unsigned int y, unsigned int z) {
            return m_blocks[x + (y * 8) + (z * 64)];
        }

        void bind() {
            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        }

        void unbind() {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void send_to_gpu(float* vertices_buffer, float x, float y, float z) {
            render_inside(vertices_buffer, x, y, z);

            bind();
            
            // send data to gpu
            glBufferData(GL_ARRAY_BUFFER, m_vbo_length * sizeof(float), m_vbo_data, GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ebo_length * sizeof(unsigned int), m_ebo_data, GL_DYNAMIC_DRAW);
            
            // setup vertex buffer layout
            // positions
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // texture coordinates
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            unbind();

            delete[] m_vbo_data;
            delete[] m_ebo_data;
        }

        void draw() {
            glDrawElements(GL_TRIANGLES, m_ebo_length, GL_UNSIGNED_INT, 0);
        }

        void uninitialize() {
            glDeleteBuffers(1, &m_ebo);
            glDeleteBuffers(1, &m_vbo);
            glDeleteVertexArrays(1, &m_vao);
        }
    };

    class chunk_side_88 {
        const unsigned short m_side_length = 8;
        GLuint m_vao, m_vbo, m_ebo;
        unsigned long long m_vbo_length, m_ebo_length;
        float* m_vbo_data;
        unsigned int* m_ebo_data;
        st2 m_side;

    public:
        chunk_side_88() {
            m_vao = 0;
            m_vbo = 0;
            m_ebo = 0;
            m_vbo_length = 0;
            m_ebo_length = 0;
            m_vbo_data = 0;
            m_ebo_data = 0;
        }

    private:
        void write_vertex(float* vertices, unsigned int index, float x, float y, float z, float side_length, tvt texture_coord) {
            vertices[index] = x;
            vertices[index + 1] = y;
            vertices[index + 2] = z;

            if (texture_coord == tvt::tvt_bottom_left) {
                vertices[index + 3] = 0.0f;
                vertices[index + 4] = 0.0f;
            } else if (texture_coord == tvt::tvt_bottom_right) {
                vertices[index + 3] = 1.0f;
                vertices[index + 4] = 0.0f;
            } else if (texture_coord == tvt::tvt_top_left) {
                vertices[index + 3] = 0.0f;
                vertices[index + 4] = 1.0f;
            } else if (texture_coord == tvt::tvt_top_right) {
                vertices[index + 3] = 1.0f;
                vertices[index + 4] = 1.0f;
            }
        }

        void write_vertex_on_cube(float* vertices, unsigned int* index, float x, float y, float z, float l, tvt texture_vertex_type, cvt cube_vertex_type) {
            if (cube_vertex_type == cvt::cvt_bottom_left_front) {
                write_vertex(vertices, *index, x, y, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_bottom_right_front) {
                write_vertex(vertices, *index, x + l, y, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_left_front) {
                write_vertex(vertices, *index, x, y + l, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_right_front) {
                write_vertex(vertices, *index, x + l, y + l, z, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_bottom_left_back) {
                write_vertex(vertices, *index, x, y, z - l, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_bottom_right_back) {
                write_vertex(vertices, *index, x + l, y, z - l, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_left_back) {
                write_vertex(vertices, *index, x, y + l, z - l, l, texture_vertex_type);
            } else if (cube_vertex_type == cvt::cvt_top_right_back) {
                write_vertex(vertices, *index, x + l, y + l, z - l, l, texture_vertex_type);
            }

            *index += 5;
        }

        void write_face(float* vertices, unsigned int* index, float x, float y, float z, float l, st2 surface_type) {
            cvt front[] = {
                cvt::cvt_bottom_left_front,
                cvt::cvt_bottom_right_front,
                cvt::cvt_top_left_front,
                cvt::cvt_top_right_front
            };

            cvt bottom[] = {
                cvt::cvt_bottom_left_front,
                cvt::cvt_bottom_right_front,
                cvt::cvt_bottom_left_back,
                cvt::cvt_bottom_right_back
            };

            cvt left[] = {
                cvt::cvt_bottom_left_front,
                cvt::cvt_bottom_left_back,
                cvt::cvt_top_left_front,
                cvt::cvt_top_left_back
            };

            cvt back[] = {
                cvt::cvt_bottom_left_back,
                cvt::cvt_bottom_right_back,
                cvt::cvt_top_left_back,
                cvt::cvt_top_right_back
            };

            cvt top[] = {
                cvt::cvt_top_left_front,
                cvt::cvt_top_right_front,
                cvt::cvt_top_left_back,
                cvt::cvt_top_right_back
            };

            cvt right[] = {
                cvt::cvt_bottom_right_front,
                cvt::cvt_bottom_right_back,
                cvt::cvt_top_right_front,
                cvt::cvt_top_right_back
            };
            
            switch (surface_type) {
            case st2::st2_front:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, front[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, front[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, front[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, front[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, front[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, front[2]);
                break;
            case st2::st2_bottom:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, bottom[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, bottom[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, bottom[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, bottom[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, bottom[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, bottom[2]);
                break;
            case st2::st2_left:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, left[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, left[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, left[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, left[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, left[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, left[2]);
                break;
            case st2::st2_back:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, back[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, back[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, back[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, back[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, back[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, back[2]);
                break;
            case st2::st2_top:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, top[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, top[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, top[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, top[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, top[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, top[2]);
                break;
            case st2::st2_right:
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_left, right[0]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, right[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, right[2]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_right, right[3]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_bottom_right, right[1]);
                write_vertex_on_cube(vertices, index, x, y, z, l, tvt::tvt_top_left, right[2]);
                break;
            }
        }

        void render_outside(float* points, chunk_888* chunk_1, chunk_888* chunk_2, st2 middle_side) {
            float side_length = 1.0f / 8.0f;
            unsigned int points_index = 0;
            unsigned int vbo_index = 0;
            unsigned int ebo_index = 0;

            // generate side
            switch (middle_side) {
            case st2::st2_front:
                for (unsigned int x = 0; x < m_side_length; x++) {
                    for (unsigned int y = 0; y < m_side_length; y++) {
                        if ((chunk_1->get_block_at(x, y, m_side_length - 1) > 0) != (chunk_2->get_block_at(x, y, 0) > 0)) {
                            write_face(points, &points_index, side_length * (float)x, side_length * (float)y, 1.0f - side_length, side_length, st2::st2_front);
                        }
                    }
                }
                break;
            case st2::st2_back:
                for (unsigned int x = 0; x < m_side_length; x++) {
                    for (unsigned int y = 0; y < m_side_length; y++) {
                        if ((chunk_1->get_block_at(x, y, 0) > 0) != (chunk_2->get_block_at(x, y, m_side_length - 1) > 0)) {
                            write_face(points, &points_index, side_length * (float)x, side_length * (float)y, 0.0f, side_length, st2::st2_back);
                        }
                    }
                }
                break;
            case st2::st2_top:
                for (unsigned int x = 0; x < m_side_length; x++) {
                    for (unsigned int z = 0; z < m_side_length; z++) {
                        if ((chunk_1->get_block_at(x, m_side_length - 1, z) > 0) != (chunk_2->get_block_at(x, 0, z) > 0)) {
                            write_face(points, &points_index, side_length * (float)x, 1.0f - side_length, side_length * (float)z, side_length, st2::st2_top);
                        }
                    }
                }
                break;
            case st2::st2_bottom:
                for (unsigned int x = 0; x < m_side_length; x++) {
                    for (unsigned int z = 0; z < m_side_length; z++) {
                        if ((chunk_1->get_block_at(x, 0, z) > 0) != (chunk_2->get_block_at(x, m_side_length - 1, z) > 0)) {
                            write_face(points, &points_index, side_length * (float)x, 0.0f, side_length * (float)z, side_length, st2::st2_bottom);
                        }
                    }
                }
                break;
            case st2::st2_left:
                for (unsigned int y = 0; y < m_side_length; y++) {
                    for (unsigned int z = 0; z < m_side_length; z++) {
                        if ((chunk_1->get_block_at(0, y, z) > 0) != (chunk_2->get_block_at(m_side_length - 1, y, z) > 0)) {
                            write_face(points, &points_index, 0.0f, side_length * (float)y, side_length * (float)z, side_length, st2::st2_left);
                        }
                    }
                }
                break;
            case st2::st2_right:
                for (unsigned int y = 0; y < m_side_length; y++) {
                    for (unsigned int z = 0; z < m_side_length; z++) {
                        if ((chunk_1->get_block_at(m_side_length - 1, y, z) > 0) != (chunk_2->get_block_at(0, y, z) > 0)) {
                            write_face(points, &points_index, 1.0f - side_length, side_length * (float)y, side_length * (float)z, side_length, st2::st2_right);
                        }
                    }
                }
                break;
            }

            // compress vbo data
            // TODO

            // write to m_vbo_data
            m_vbo_length = points_index;
            m_vbo_data = new float[m_vbo_length];

            for (unsigned int i = 0; i < m_vbo_length; i++) {
                m_vbo_data[i] = points[i];
            }

            // write ebo data
            m_ebo_length = points_index / 5;
            m_ebo_data = new unsigned int[m_ebo_length];

            for (unsigned int i = 0; i < m_ebo_length; i++) {
                m_ebo_data[i] = i;
            }
        }

    public:
        void initialize() {
            // setup opengl buffers
            glGenVertexArrays(1, &m_vao);
            glGenBuffers(1, &m_vbo);
            glGenBuffers(1, &m_ebo);
        }

        void bind() {
            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        }

        void unbind() {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        void send_to_gpu(float* vertices_buffer, chunk_888* chunk_1, chunk_888* chunk_2, st2 middle_side) {
            render_outside(vertices_buffer, chunk_1, chunk_2, middle_side);

            bind();
            
            // send data to gpu
            glBufferData(GL_ARRAY_BUFFER, m_vbo_length * sizeof(float), m_vbo_data, GL_DYNAMIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ebo_length * sizeof(unsigned int), m_ebo_data, GL_DYNAMIC_DRAW);
            
            // setup vertex buffer layout
            // positions
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // texture coordinates
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            unbind();

            delete[] m_vbo_data;
            delete[] m_ebo_data;
        }

        void draw() {
            glDrawElements(GL_TRIANGLES, m_ebo_length, GL_UNSIGNED_INT, 0);
        }

        void uninitialize() {
            glDeleteBuffers(1, &m_ebo);
            glDeleteBuffers(1, &m_vbo);
            glDeleteVertexArrays(1, &m_vao);
        }
    };
}