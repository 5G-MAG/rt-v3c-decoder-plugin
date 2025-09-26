/*
* Copyright (c) 2025 InterDigital CE Patent Holdings SASU
* Licensed under the License terms of 5GMAG software (the "License").
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at https://www.5g-mag.com/license .
* Unless required by applicable law or agreed to in writing, software distributed under the License is
* distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and limitations under the License.
*/

#include "VPCCModelBuilder.h"

VPCCModelBuilder::VPCCModelBuilder(bool render_shadow, bool count_points)
    : VPCCRenderer(render_shadow, render_shadow ? "Model Builder Shadow" : "Model Builder", count_points)
{
    printComputeCapabilities();
    create_model_builder_program();
}
//
//void VPCCModelBuilder::create_decimation_program() {
//    m_compute_decimation_program = Program(LoadShader("decimate.glsl"));
//    LogError("Decimation Program");
//}


void VPCCModelBuilder::create_model_builder_program() {
    
    static const std::string clear_image_compute_src = R"DEF_SHADER(
    
    layout (local_size_x = 32, local_size_y = 32) in;

    layout(rgba32f) uniform writeonly image2D img;
    //uniform vec4 clear_color;
    
    const vec4 clear_color = vec4(0.0f,0.0f,0.0f,0.0f);

    void main() {
        ivec2 id = ivec2(gl_GlobalInvocationID.xy);
        imageStore(img, id.xy, clear_color); 
    }

    )DEF_SHADER";

    std::string shader_code;
    std::string shader_code_dd;
    //shader_code = LoadShader("modelBuilderDecimate.glsl");
    #pragma region ModelBuilderShader
    shader_code = R"DEF_SHADER( 
    layout (local_size_x = 16, local_size_y = 16) in;

    uniform int width;
    uniform int height;
    uniform uint vert_incr;
    uniform float norm_res_factor;
    //@if_shadow uniform float shadow_res_factor;
    uniform sampler2D tex_col;
    uniform sampler2D tex_geo;
    uniform sampler2D tex_ocm;
    uniform sampler2D tex_uv0uv1;
    uniform sampler2D tex_d1nop;
    //@if_dynamic_decimation uniform usampler2D tex_dec;   
    //@if_global_decimation uniform uint decimation_level;
    uniform float N_off;
    uniform float B_off;
    uniform float T_off;

    layout(rgba32f, binding = 0) uniform writeonly image2D pos_img;
    layout(rgba32f, binding = 1) uniform writeonly image2D col_img;
    //@if_shadow layout(rgba32f, binding = 2) uniform writeonly image2D shadow_img;
     
    layout(std430, binding = 3) buffer ssbo{
        uint num_vertex;
    }; 

    const mat3 rot1 = mat3 (1, 0, 0,
                            0, 1, 0,
                            0, 0, 1);
    const mat3 rot2 = mat3 (0, 1, 0,
                            1, 0, 0,
                            0, 0, 1);
    const mat3 rot3 = mat3 (0, 0, 1,
                            0, 1, 0,
                            1, 0, 0);

    const vec4 default_pos = vec4(-2.0f,-2.0f,-2.0f,0.0f);
    
    void main() {
 
        uvec2 id = uvec2(gl_GlobalInvocationID.xy);
        vec2 uv = vec2((float(id.x)+0.5f)/float(width), (float(id.y)+0.5f)/float(height));
        vec2 inv_uv = vec2(uv.x,1.0f-uv.y);

        //@if_dynamic_decimation uvec4 decimation_vec = textureLod(tex_dec,uv.xy, 0.0);
        //@if_dynamic_decimation uint decimation_level = decimation_vec.x;

        vec4 ocm = textureLod(tex_ocm, inv_uv.xy, 0.0)*255.0;

        float is_point = float(ocm.r > 0.5 && (id.x%decimation_level) == 0u && (id.y%decimation_level) == 0u);
    
        if (is_point> 0.5){
            vec4 col = textureLod(tex_col, inv_uv.xy, 0.0);
            vec4 geo = textureLod(tex_geo, inv_uv.xy, 0.0);
    

            //Load Metada
            vec4 u0v0_u1v1_t = textureLod(tex_uv0uv1, uv.xy, 0.0);
            vec4 d1_norm_orient_proj_t = textureLod(tex_d1nop, uv.xy, 0.0);

            uint u0 = uint(u0v0_u1v1_t.x);
            uint v0 = uint(u0v0_u1v1_t.y);
            float u1 = u0v0_u1v1_t.z;
            float v1 = u0v0_u1v1_t.a;
            float d1 = d1_norm_orient_proj_t.x;
            uint normal = uint(d1_norm_orient_proj_t.y);
            uint orient = uint(d1_norm_orient_proj_t.z);
            int proj_mode = int(d1_norm_orient_proj_t.a);
    

            //Orientation        
            mat3 switch_mat = float(normal == 0u) * rot1 + float(normal == 1u) * rot2 + float(normal == 2u) * rot3;

            int proj = 255 * (-2 * proj_mode + 1);

            // Compute position
            float origin_x = float(id.x - u0 * 16u); //U0 and V0 are in VPCC block coordinates, so we need to scale them up
            float origin_y = float(id.y - v0 * 16u);
        
            //Compute T/B positions
            //Shenanigans to avoid if else statements
            float origin[2];
            origin[0] = origin_x;
            origin[1] = origin_y;
            float T = origin[orient] + u1; //Orientation is 0 (no rotation) or 1 (90 deg rotation), so this work
            float B = origin[uint(1)-orient] + v1;

            //Compute N0 position
	        float N0 = d1 + geo.r * float(proj);
            vec3 pos = switch_mat * vec3(N0 + N_off * float(proj), B + B_off, T + T_off);
            vec4 pos_col = vec4(pos*norm_res_factor, float(decimation_level));
        
            uint update_num_vert = atomicAdd(num_vertex, vert_incr);
            uint atomic_lid = update_num_vert / vert_incr;

            ivec2 atomic_id = ivec2(atomic_lid%uint(width), atomic_lid/uint(width));
            imageStore(pos_img, atomic_id.xy, pos_col);
            imageStore(col_img, atomic_id.xy, vec4(col.xyz, 1.0));
            //@if_shadow imageStore(shadow_img, ivec2(pos.xz*shadow_res_factor), vec4(0.0,0.0,0.0,1.0));
        }
    }
    )DEF_SHADER";
#pragma endregion

    #pragma region DecimationShader
    std::string decimation_code = R"DEF_SHADER(
    layout (local_size_x = 16, local_size_y = 16) in;

    uniform int block_w;
    uniform int block_h;
    uniform mat4 MVP;
    uniform float norm_res_factor;
    uniform sampler2D tex_geo;
    uniform sampler2D tex_uv0uv1;
    uniform sampler2D tex_d1nop;
    uniform float r1;
    uniform float r2;
    uniform float vp_cull_factor;

    layout(rgba8ui, binding = 0) uniform writeonly uimage2D dec_img;

    const mat3 rot1 = mat3 (1, 0, 0,
                            0, 1, 0,
                            0, 0, 1);
    const mat3 rot2 = mat3 (0, 1, 0,
                            1, 0, 0,
                            0, 0, 1);
    const mat3 rot3 = mat3 (0, 0, 1,
                            0, 1, 0,
                            1, 0, 0);

    const vec4 default_pos = vec4(-2.0f,-2.0f,-2.0f,0.0f);

    void main() {

        uvec2 id = gl_GlobalInvocationID.xy;
        vec2 uv = vec2((float(id.x)+0.5f)/float(block_w), (float(id.y)+0.5f)/float(block_h));
        //vec2 uv = vec2((float(id.x)+0.03f)/float(block_w), (float(id.y)+0.03f)/float(block_h));
        vec2 inv_uv = vec2(uv.x,1.0f-uv.y);

        vec4 geo = textureLod(tex_geo, inv_uv.xy, 0.0);

        //Load Metada
        vec4 u0v0_u1v1_t = textureLod(tex_uv0uv1, uv.xy, 0.0);
        vec4 d1_norm_orient_proj_t = textureLod(tex_d1nop, uv.xy, 0.0);

        uint u0 = uint(u0v0_u1v1_t.x);
        uint v0 = uint(u0v0_u1v1_t.y);
        float u1 = u0v0_u1v1_t.z;
        float v1 = u0v0_u1v1_t.a;
        float d1 = d1_norm_orient_proj_t.x;
        uint normal = uint(d1_norm_orient_proj_t.y);
        uint orient = uint(d1_norm_orient_proj_t.z);
        int proj_mode = int(d1_norm_orient_proj_t.a);

        //Orientation
        mat3 switch_mat = float(normal == 0u) * rot1 + float(normal == 1u) * rot2 + float(normal == 2u) * rot3;

        int proj = 255 * (-2 * proj_mode + 1);

        // Compute position
        float origin_x = float((id.x - u0) * 16u); //ID, U0 and V0 are in VPCC block coordinates, so we need to scale them up
        float origin_y = float((id.y - v0) * 16u);

        //Compute T/B positions
        //Shenanigans to avoid if else statements
        float origin[2];
        origin[0] = origin_x;
        origin[1] = origin_y;
        float T = origin[orient] + u1; //Orientation is 0 (no rotation) or 1 (90 deg rotation), so this work
        float B = origin[uint(1)-orient] + v1;

        //Compute N0 position
        float N0 = d1 + geo.r * float(proj);
        vec3 pos = (switch_mat * vec3(N0, B, T))* norm_res_factor; //Model space point position
        vec4 p_pos = MVP * vec4(pos.xyz, 1.0 ); //Projection space position
        vec4 n_pos = p_pos/p_pos.w; // Normalized projection space position (-1 to 1)

        //Compute Decimation
        float depth = p_pos.z;
        //Decimation from depth
        uint dec = uint(depth < r1) * 1u + uint((depth > r1) && (depth < r2 )) * 2u + uint(depth > r2)* 4u;
        //Cull points not in viewport
        uint dec_c = dec*uint((abs(n_pos.x) < vp_cull_factor) && (abs(n_pos.y) <vp_cull_factor));
        imageStore(dec_img, ivec2(int(id.x), int(id.y)), uvec4(dec_c, 0u, 0u, 0u));
        //imageStore(dec_img, ivec2(int(id.x), int(id.y)), uvec4(2u, 0u, 0u, 0u));
    }

    )DEF_SHADER";
#pragma endregion


#pragma region DecimationShader2
    std::string decimation_code_2 = R"DEF_SHADER(
    layout (local_size_x = 16, local_size_y = 16) in;

    uniform int block_w;
    uniform int block_h;
    uniform mat4 MVP;
    uniform float norm_res_factor;
    uniform sampler2D tex_geo;
    uniform sampler2D tex_uv0uv1;
    uniform sampler2D tex_d1nop;
    uniform float r1;
    uniform float r2;
    uniform float vp_cull_factor;

    layout(rgba8ui, binding = 0) uniform writeonly uimage2D dec_img;

    const mat3 rot1 = mat3 (1, 0, 0,
                            0, 1, 0,
                            0, 0, 1);
    const mat3 rot2 = mat3 (0, 1, 0,
                            1, 0, 0,
                            0, 0, 1);
    const mat3 rot3 = mat3 (0, 0, 1,
                            0, 1, 0,
                            1, 0, 0);

    const vec4 default_pos = vec4(-2.0f,-2.0f,-2.0f,0.0f);

    void main() {

        uvec2 id = gl_GlobalInvocationID.xy;
        vec2 uv = vec2((float(id.x)+0.5f)/float(block_w), (float(id.y)+0.5f)/float(block_h));
        //vec2 uv = vec2((float(id.x)+0.03f)/float(block_w), (float(id.y)+0.03f)/float(block_h));
        vec2 inv_uv = vec2(uv.x,1.0f-uv.y);

        vec4 geo = textureLod(tex_geo, inv_uv.xy, 0.0);

        //Load Metada
        vec4 u0v0_u1v1_t = textureLod(tex_uv0uv1, uv.xy, 0.0);
        vec4 d1_norm_orient_proj_t = textureLod(tex_d1nop, uv.xy, 0.0);

        uint u0 = uint(u0v0_u1v1_t.x);
        uint v0 = uint(u0v0_u1v1_t.y);
        float u1 = u0v0_u1v1_t.z;
        float v1 = u0v0_u1v1_t.a;
        float d1 = d1_norm_orient_proj_t.x;
        uint normal = uint(d1_norm_orient_proj_t.y);
        uint orient = uint(d1_norm_orient_proj_t.z);
        int proj_mode = int(d1_norm_orient_proj_t.a);

        //Orientation
        mat3 switch_mat = float(normal == 0u) * rot1 + float(normal == 1u) * rot2 + float(normal == 2u) * rot3;

        int proj = 255 * (-2 * proj_mode + 1);

        // Compute position
        float origin_x = float((id.x - u0) * 16u); //ID, U0 and V0 are in VPCC block coordinates, so we need to scale them up
        float origin_y = float((id.y - v0) * 16u);

        //Compute T/B positions
        //Shenanigans to avoid if else statements
        float origin[2];
        origin[0] = origin_x;
        origin[1] = origin_y;
        float T = origin[orient] + u1; //Orientation is 0 (no rotation) or 1 (90 deg rotation), so this work
        float B = origin[uint(1)-orient] + v1;

        //Compute N0 position
        float N0 = d1 + geo.r * float(proj);
        vec3 pos = (switch_mat * vec3(N0, B, T))* norm_res_factor; //Model space point position
        vec3 pos1 = (switch_mat * vec3(N0, B+1.0f, T+1.0f))* norm_res_factor; //Model space point position
        vec4 p_pos = MVP * vec4(pos.xyz, 1.0f); //Projection space position
        vec4 p_pos1 = MVP * vec4(pos1.xyz, 1.0f); //Projection space position      
        vec4 n_pos = p_pos/p_pos.w; // Normalized projection space position (-1 to 1)        
        vec4 n_pos1 = p_pos1/p_pos1.w; // Normalized projection space position (-1 to 1)        
        float dist = length((n_pos.xyz-n_pos1.xyz));
        

        

        //Compute Decimation
        
        //Decimation from depth
        //uint dec = 2;
        uint dec = uint(dist < r1) * 4u + uint((dist > r1) && (dist < r2 )) * 2u + uint(dist > r2)* 1u;
        //Cull points not in viewport
        uint dec_c = dec*uint((abs(n_pos.x) < vp_cull_factor) && (abs(n_pos.y) <vp_cull_factor));
        imageStore(dec_img, ivec2(int(id.x), int(id.y)), uvec4(dec_c, 0u, 0u, 0u));
        //imageStore(dec_img, ivec2(int(id.x), int(id.y)), uvec4(2u, 0u, 0u, 0u));
    }

    )DEF_SHADER";
#pragma endregion

    m_compute_decimation_program = Program(decimation_code_2);
    //if (m_useDD) {
        //m_compute_decimation_program = Program(LoadShader("decimate.glsl"));
        shader_code_dd = iloj::misc::replace(shader_code, { {"//@if_dynamic_decimation", ""} });
        //LogError("Decimation Program");
    //}
    //else {
        shader_code = iloj::misc::replace(shader_code, { {"//@if_global_decimation", ""} });
    //}

    if (m_renderShadow) {
        m_clear_image_program = Program(clear_image_compute_src);
        LogError("Clear Program");
        m_model_builder_program = Program(iloj::misc::replace(shader_code, {{"//@if_shadow", ""}}));
        m_model_builder_program_dd = Program(iloj::misc::replace(shader_code_dd, {{"//@if_shadow", ""}}));
        LogError("Model Builder Program (Shadow)");
    }
    else {
        m_model_builder_program = Program(shader_code);
        m_model_builder_program_dd = Program(shader_code_dd);
        LogError("Model Builder Program (No Shadow)");
    }
}

void VPCCModelBuilder::render(const Metadata &metaData,
                              const iloj::gpu::Texture2D &occupancyMap,
                              const iloj::gpu::Texture2D &geometryMap,
                              const iloj::gpu::Texture2D &textureMap,
                              const iloj::gpu::Texture2D &transparencyMap,
                              iloj::gpu::Texture2D &canvas)

{
    build_model(metaData, occupancyMap, geometryMap, textureMap, canvas);
}

void VPCCModelBuilder::build_model(const Metadata& metaData,
    const iloj::gpu::Texture2D& occupancyMap,
    const iloj::gpu::Texture2D& geometryMap,
    const iloj::gpu::Texture2D& textureMap,
    iloj::gpu::Texture2D& canvas)

{
    
    int width = textureMap.width();
    int height = textureMap.height();
    int dec_w = width / m_decimationLevel;
    int dec_h = height / m_decimationLevel;
    float norm_factor = 1.0f / m_maxBbox;
    bool size_changed = width != m_widthInit || height != m_heightInit;
    m_widthInit = width;
    m_heightInit = height;
    

    //Shadow stuff
    bool shadow_tex_valid = true;
    int shadow_width=0, shadow_height=0;
    float shadow_factor=1;

    if (m_renderShadow) 
    {
        //LOG_INFO("Shadow rendering setup");
        shadow_width = m_shadowTex.width();
        shadow_height = m_shadowTex.height();
        shadow_factor = shadow_width / m_maxBbox;
        shadow_tex_valid = shadow_width > 0 && shadow_height > 0;
    }

    Program& builder = m_useDD ? m_model_builder_program_dd : m_model_builder_program;
    
    if (shadow_tex_valid && (!m_isTexInit || size_changed))
    {
        // Create the buffers only once
        if (!m_isTexInit)
        {
            //LOG_INFO("Init Atomics");
            glGenBuffers(1, &m_atomicsBuffer);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_atomicsBuffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
            LogError("Init Atomics");
        }

        //m_decimationTex = std::make_unique<Texture2D>(width / 16, height / 16, GL_RGBA8UI, GL_NEAREST, GL_CLAMP_TO_EDGE, true);
        //m_decimationTex = Texture2D(width / 16, height / 16, GL_RGBA8UI, GL_NEAREST, GL_CLAMP_TO_EDGE, false);
        initTextureGLES(m_decimationTex, width / 16, height / 16, GL_RGBA8UI, GL_NEAREST, GL_CLAMP_TO_EDGE);      
        LogError("Reshape Decimation Tex");
    


        // Note: OpenGL ES don't seem to support READ_WRITE, so you have to choose between READ_ONLY and WRITE_ONLY
        m_posImg = std::make_unique<ImageES>(m_posTex, GL_WRITE_ONLY); 
        m_colImg = std::make_unique<ImageES>(canvas, GL_WRITE_ONLY);
        LogError("Set color and position Images");
        
        if (m_renderShadow) {
            m_shadowImg = std::make_unique<ImageES>(m_shadowTex, GL_WRITE_ONLY);
            LogError("Set shadow Image");
        }
        
        m_decImg = std::make_unique<ImageES>(m_decimationTex, GL_WRITE_ONLY);
        LogError("Set decimation Image");

        m_isTexInit = true;
        LogError("Init Images");
    }

    bool is_new_frame = metaData.VPCCMetadata->frame_index != m_lastFrameId;
    //Load metadata if needed
    if (m_isTexInit && is_new_frame)
    {
        //LOG_INFO("Format Metadata");
        m_lastFrameId = metaData.VPCCMetadata->frame_index;
        // Getting the metadata
        formatMetadata(metaData.VPCCMetadata, width, height);
        LogError("Post Format Metadata");
    }

    // Model building, done only once per video frame (except if forced)
    if (m_isTexInit && (m_useDD || m_forceRebuild || is_new_frame))
    {
        if (m_renderShadow)
        {
            //LOG_INFO("Shadow clear");
            execute(Vec3u{ (unsigned int)(shadow_width / 32), (unsigned int)(shadow_height / 32), 1 },
                GL_ALL_BARRIER_BITS,
                m_clear_image_program,
                Uniform::Entry<ImageES>("img", *m_shadowImg));
            LogError("Post Clear Shadow");
        }

        if (m_useDD) {
            //LOG_INFO("Decimation computing");
            execute(Vec3u{ (unsigned int)(width / 256 + 1), (unsigned int)(height / 256 + 1), 1 },
                GL_ALL_BARRIER_BITS,
                m_compute_decimation_program,
                Uniform::Entry<ImageES>("dec_img", *m_decImg),
                Uniform::Entry<int>("block_w", width/16),
                Uniform::Entry<int>("block_h", height/16),
                Uniform::Entry<float>("norm_res_factor", norm_factor),
                Uniform::Entry<float>("r1", m_r1),
                Uniform::Entry<float>("r2", m_r2),
                Uniform::Entry<float>("vp_cull_factor", m_vp_cull_factor),
                Uniform::Entry<Mat4x4f>("MVP", m_MVP),
                Uniform::Entry<Texture2D>("tex_geo", geometryMap),
                Uniform::Entry<Texture2D>("tex_uv0uv1", m_texUV0UV1),
                Uniform::Entry<Texture2D>("tex_d1nop", m_texD1NOP));
            LogError("Execute Decimation");
        }

        //LOG_INFO("Build Model");
        // Compute shader setup
        Vec3u num_threads{ (unsigned int)(width / 16 + 1),
                          (unsigned int)(height / 16 + 1),
                          1 }; // Ceilling to take all points into account

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_atomicsBuffer);
        GLuint a[1] = { 0 };
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), a);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        LogError("Post Atomic Init");

        builder.bind();

        builder.setUniform(
            Uniform::Entry<ImageES>("pos_img", *m_posImg),
            Uniform::Entry<ImageES>("col_img", *m_colImg));
        
        if (m_renderShadow)
        {
            // Iloj implementation for setting uniforms allocates a binding slot dynamically,
            // following the call order.  To keep the bindings defined in the shader, we 
            // must set the images in the right order
            // (hence the weird position of this code snippet)

            builder.setUniform(
                Uniform::Entry<ImageES>("shadow_img", *m_shadowImg),
                Uniform::Entry<float>("shadow_res_factor", shadow_factor));
        }

        builder.setUniform(
                Uniform::Entry<Texture2D>("tex_col", textureMap),
                Uniform::Entry<Texture2D>("tex_geo", geometryMap),
                Uniform::Entry<Texture2D>("tex_ocm", occupancyMap),
                Uniform::Entry<Texture2D>("tex_uv0uv1", m_texUV0UV1),
                Uniform::Entry<Texture2D>("tex_d1nop", m_texD1NOP),
                Uniform::Entry<GLuint>("vert_incr", GLuint(m_numVertPerPoint)),
                Uniform::Entry<float>("norm_res_factor", norm_factor),
                Uniform::Entry<int>("width", width),
                Uniform::Entry<int>("height", height),
                Uniform::Entry<float>("N_off", m_NOff),
                Uniform::Entry<float>("B_off", m_BOff),
                Uniform::Entry<float>("T_off", m_TOff));

        if (m_useDD) {
            m_model_builder_program.setUniform(Uniform::Entry<Texture2D>("tex_dec", m_decimationTex));
        }
        else {
            m_model_builder_program.setUniform(Uniform::Entry<GLuint>("decimation_level", GLuint(m_decimationLevel)));
        }
        LogError("Post Uniform");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_atomicsBuffer);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glDispatchCompute(num_threads[0], num_threads[1], num_threads[2]);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        builder.unbind();

        LogError("Post Compute");

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_atomicsBuffer);
        glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER,
            GL_DRAW_INDIRECT_BUFFER,
            0,
            0,
            sizeof(GLuint)); // Copy from SSBO[0] to Indirect[0],

        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        LogError("END");
    }
}

std::string VPCCModelBuilder::LoadShader(const std::string path) {
    const std::string src_path(__FILE__);
    const std::string shader_dir = src_path.substr(0, src_path.find_last_of("\\")) + "\\..\\";
    std::string full_path = shader_dir + path;
    LOG_INFO("Reading shader at path ", full_path);
    //std::ifstream t(full_path);
    //std::stringstream buffer;
    //buffer << t.rdbuf();
    ////LOG_INFO(buffer.str());
    //return buffer.str();
    return "";
}

void VPCCModelBuilder::printComputeCapabilities()
{
    GLint max_tex_image_units, max_total_work_groups, max_x_work_groups, max_y_work_groups, max_z_work_groups;
    glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_tex_image_units);
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_total_work_groups);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &max_x_work_groups);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &max_y_work_groups);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &max_z_work_groups);

    LOG_INFO("OpengGl Compute Shader Capabilities: Max Total Work Groups Size = ",
             max_total_work_groups,
             ": [",
             max_x_work_groups,
             ", ",
             max_y_work_groups,
             ", ",
             max_z_work_groups,
             "] ",
             ", Max Image/Texture Slots = ",
             max_tex_image_units);
}
