#pragma once

#include "../../utils/GLHeadersAndMacros.h"
#include "../../utils/GLShaders.h"
#include "../../utils/common.h"
#include "../../utils/OpenGL/DDSLoader.h"
#include "Matrix.h"

using namespace MaliSDK;

class Metaball
{

public:
    // Member Variables
    GLuint vao_fire;
    GLuint vbo_position_fire;
    GLuint vbo_texcoord_fire;

    GLuint shaderProgramObject_sphere_updater;
    GLuint timeUniform;

    GLuint shaderProgramObject_scaler_field;
    GLuint samplerPerAxisUniform;
    GLuint sphereUniformBlocks;

    GLuint shaderProgramObject_marching_cubes;
    GLuint scalerFieldSamplerUniform_cubes;
    GLuint cellsPerAxisUniform;
    GLuint isoLevelUniform_cubes;

    GLuint shaderProgramObject_marching_cube_triangles;
    GLuint samplesPerAxisUniform;
    GLuint cellTypesSamplerUniform;
    GLuint scalerFieldSamplerUniform;
    GLuint triTableSamplerUniform;
    GLuint mvpUniform;
    GLuint isoLevelUniform;
    GLuint timeMarchingTriangleUniform;
    GLuint spherePositionIdUniform;

    GLuint modelMatrixUniform;
    GLuint viewMatrixUniform;
    GLuint projectionMatrixUniform;

    //
    GLuint spheres_updater_transform_feedback_object_id = 0;
    GLuint spheres_updater_sphere_positions_buffer_object_id = 0;
    GLuint scalar_field_transform_feedback_object_id = 0;
    const GLuint samples_in_3d_space = samples_per_axis * samples_per_axis * samples_per_axis; /**< Amount of samples in 3D space. */
    GLuint scalar_field_buffer_object_id = 0;
    const int n_spheres = 3;
    const int n_sphere_position_components = 4;

    const GLuint tesselation_level = 32;               /**< Level of details you would like to split model into. Please use values from th range [8..256]. */
    const GLuint samples_per_axis = tesselation_level; /**< Amount of samples we break scalar space into (per each axis). */
    /** Id of a 3D texture object storing scalar field data. */
    GLuint scalar_field_texture_object_id = 0;
    const GLuint cells_per_axis = samples_per_axis - 1; /**< Amount of cells per each axis. */
    GLfloat isosurface_level = 12.0f;                   /**< Scalar field's isosurface level.                                                               */
    /** Id of a buffer object to hold result cell type data. */
    GLuint marching_cubes_cells_types_buffer_id = 0;
    const GLuint cells_in_3d_space = cells_per_axis * cells_per_axis * cells_per_axis; /**< Amount of cells in 3D space. */
    /** Id of transform feedback object to keep cell types buffer binding. */
    GLuint marching_cubes_cells_transform_feedback_object_id = 0;
    /** Id of a texture object to hold result cell type data. */
    GLuint marching_cubes_cells_types_texture_object_id = 0;
    /** Id of a texture object to hold triangle look-up table data. */
    GLuint marching_cubes_triangles_lookup_table_texture_id = 0;
    const GLuint vertices_per_triangle = 3; /**< Amount of vertices that defines one triangle. */
    const GLuint triangles_per_cell = 5;
    const GLuint mc_vertices_per_cell = vertices_per_triangle * triangles_per_cell; /**< Amount of vertices in tri_table representing triangles by vertices for one cell. */
    const GLuint mc_cells_types_count = 256;
    
    /** Id of vertex array object. */
    GLuint marching_cubes_triangles_vao_id = 0;
    GLfloat model_time = 0.0f;
    Matrix mvp;

    GLuint spheres_updater_uniform_time_id = 0;
    const GLchar *sphere_position_varying_name = "sphere_position";
    const GLchar *scalar_field_value_varying_name = "scalar_field_value";
    /** Cell_type_index output variable's name. */
    const GLchar *marching_cubes_cells_varying_name = "cell_type_index";
    /** Name of sphere_positions_uniform_block uniform block. */
    const GLchar *marching_cubes_triangles_uniform_sphere_positions_name = "sphere_positions_uniform_block";

    /* [tri_table chosen part for documentation] */
    // const GLint tri_table[mc_cells_types_count * mc_vertices_per_cell] =
    const GLint tri_table[256 * 3 * 5] =
        {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            /* [tri_table chosen part for documentation] */
            1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1,
            3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1,
            3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1,
            3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1,
            9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1,
            1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1,
            9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1,
            2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1,
            8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1,
            9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1,
            4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1,
            3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1,
            1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1,
            4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1,
            4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1,
            9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1,
            1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1,
            5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1,
            2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1,
            9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1,
            0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1,
            2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1,
            10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1,
            4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1,
            5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1,
            5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1,
            9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1,
            0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1,
            1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1,
            10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1,
            8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1,
            2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1,
            7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1,
            9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1,
            2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1,
            11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1,
            9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1,
            5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0,
            11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0,
            11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1,
            1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1,
            9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1,
            5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1,
            2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1,
            0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1,
            5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1,
            6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1,
            0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1,
            3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1,
            6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1,
            5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1,
            1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1,
            10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1,
            6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1,
            1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1,
            8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1,
            7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9,
            3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1,
            5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1,
            0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1,
            9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6,
            8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1,
            5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11,
            0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7,
            6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1,
            10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1,
            10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1,
            8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1,
            1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1,
            3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1,
            0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1,
            10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1,
            0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1,
            3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1,
            6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1,
            9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1,
            8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1,
            3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1,
            6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1,
            0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1,
            10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1,
            10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1,
            1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1,
            2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9,
            7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1,
            7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1,
            2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7,
            1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11,
            11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1,
            8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6,
            0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1,
            7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1,
            10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1,
            2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1,
            6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1,
            7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1,
            2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1,
            1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1,
            10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1,
            10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1,
            0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1,
            7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1,
            6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1,
            8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1,
            9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1,
            6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1,
            1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1,
            4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1,
            10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3,
            8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1,
            0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1,
            1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1,
            8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1,
            10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1,
            4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3,
            10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1,
            5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1,
            11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1,
            9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1,
            6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1,
            7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1,
            3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6,
            7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1,
            9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1,
            3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1,
            6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8,
            9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1,
            1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4,
            4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10,
            7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1,
            6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1,
            3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1,
            0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1,
            6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1,
            1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1,
            0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10,
            11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5,
            6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1,
            5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1,
            9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1,
            1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8,
            1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6,
            10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1,
            0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1,
            5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1,
            10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1,
            11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1,
            0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1,
            9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1,
            7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2,
            2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1,
            8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1,
            9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1,
            9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2,
            1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1,
            9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1,
            9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1,
            5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1,
            0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1,
            10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4,
            2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1,
            0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11,
            0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5,
            9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1,
            5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1,
            3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9,
            5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1,
            8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1,
            0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1,
            9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1,
            0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1,
            1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1,
            3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4,
            4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1,
            9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3,
            11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1,
            11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1,
            2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1,
            9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7,
            3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10,
            1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1,
            4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1,
            4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1,
            0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1,
            3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1,
            3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1,
            0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1,
            9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1,
            1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};



    // Member Functions
    Bool initialize(void)
    {
        // code
        if (!initSphereUpdater())
        {
            PrintLog("Failed to initialize SphereUpdater\n");
            printf("Failed to initialize SphereUpdater\n");
            return False;
        }

        if (!initScalerField())
        {
            PrintLog("Failed to initialize ScalerField\n");
            printf("Failed to initialize ScalerField\n");
            return False;
        }

        if (!initMarchingCubeCells())
        {
            PrintLog("Failed to initialize MarchingCells\n");
            printf("Failed to initialize MarchingCells\n");
            return False;
        }

        if (!initMarchingCubeTriangles())
        {
            PrintLog("Failed to initialize MarchingCubeTriangles\n");
            printf("Failed to initialize MarchingCubeTriangles\n");
            return False;
        }

        /* Enable facet culling, depth testing and specify front face for polygons. */
        // glEnable   (GL_DEPTH_TEST);
        // glEnable   (GL_CULL_FACE );
        // glFrontFace(GL_CW        );

        return True;
    }

    Bool initSphereUpdater(void)
    {
        // GLuint spheres_updater_sphere_positions_buffer_object_id = 0;
        
        // GLuint spheres_updater_uniform_time_id = 0;
        // const GLchar *sphere_position_varying_name = "sphere_position";

        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/sphereupdater.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/sphereupdater.fs", FRAGMENT);

        shaderProgramObject_sphere_updater = glCreateProgram();

        glAttachShader(shaderProgramObject_sphere_updater, vertexShaderObject);
        glAttachShader(shaderProgramObject_sphere_updater, fragmentShaderObject);

        glTransformFeedbackVaryings(shaderProgramObject_sphere_updater, 1, &sphere_position_varying_name, GL_SEPARATE_ATTRIBS);

        // glBindAttribLocation(shaderProgramObject_sphere_updater, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_sphere_updater, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_sphere_updater);
        if (bLinkShaderStatus == False)
        {
            return False;
        }

        // map uniforms with CPU variables
        timeUniform = glGetUniformLocation(shaderProgramObject_sphere_updater, "time");

        // active the program
        glUseProgram(shaderProgramObject_sphere_updater);

        glGenBuffers(1, &spheres_updater_sphere_positions_buffer_object_id);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, spheres_updater_sphere_positions_buffer_object_id);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, n_spheres * n_sphere_position_components * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        glGenTransformFeedbacks(1, &spheres_updater_transform_feedback_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, spheres_updater_transform_feedback_object_id);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, spheres_updater_sphere_positions_buffer_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        return True;
    }

    Bool initScalerField(void)
    {
        // const GLchar *scalar_field_value_varying_name = "scalar_field_value";
        
        // const GLuint samples_in_3d_space = samples_per_axis * samples_per_axis * samples_per_axis; /**< Amount of samples in 3D space. */
        

        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/scalerfield.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/scalerfield.fs", FRAGMENT);

        shaderProgramObject_scaler_field = glCreateProgram();

        glAttachShader(shaderProgramObject_scaler_field, vertexShaderObject);
        glAttachShader(shaderProgramObject_scaler_field, fragmentShaderObject);

        // glBindAttribLocation(shaderProgramObject_scaler_field, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_scaler_field, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        glTransformFeedbackVaryings(shaderProgramObject_scaler_field, 1, &scalar_field_value_varying_name, GL_SEPARATE_ATTRIBS);

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_scaler_field);
        if (bLinkShaderStatus == False)
        {
            return False;
        }

        // map uniforms with CPU variables
        samplerPerAxisUniform = glGetUniformLocation(shaderProgramObject_scaler_field, "samples_per_axis");
        sphereUniformBlocks = glGetUniformBlockIndex(shaderProgramObject_scaler_field, "spheres_uniform_block");

        glUseProgram(shaderProgramObject_scaler_field);

        glUniform1i(shaderProgramObject_scaler_field, samplerPerAxisUniform);

        glUniformBlockBinding(shaderProgramObject_scaler_field, sphereUniformBlocks, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, spheres_updater_sphere_positions_buffer_object_id);

        glGenBuffers(1, &scalar_field_buffer_object_id);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, scalar_field_buffer_object_id);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, samples_in_3d_space * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        glGenTransformFeedbacks(1, &scalar_field_transform_feedback_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, scalar_field_buffer_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* [Stage 2 Creating texture] */
        /* Generate texture object to hold scalar field data. */
        glGenTextures(1, &scalar_field_texture_object_id);

        /* Scalar field uses GL_TEXTURE_3D target of texture unit 1. */
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, scalar_field_texture_object_id);

        /* Prepare texture storage for scalar field values. */
        glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32F, samples_per_axis, samples_per_axis, samples_per_axis);
        /* [Stage 2 Creating texture] */

        /* Tune texture settings to use it as a data source. */
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return True;
    }

    Bool initMarchingCubeCells(void)
    {
        // /** Cell_type_index output variable's name. */
        // const GLchar *marching_cubes_cells_varying_name = "cell_type_index";

        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubescells.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubescells.fs", FRAGMENT);

            printf("shader compilation done\n");

        shaderProgramObject_marching_cubes = glCreateProgram();

        glAttachShader(shaderProgramObject_marching_cubes, vertexShaderObject);
        glAttachShader(shaderProgramObject_marching_cubes, fragmentShaderObject);

        // glBindAttribLocation(shaderProgramObject_marching_cubes, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_marching_cubes, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        glTransformFeedbackVaryings(shaderProgramObject_marching_cubes, 1, &marching_cubes_cells_varying_name, GL_SEPARATE_ATTRIBS);

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_marching_cubes);
        if (bLinkShaderStatus == False)
        {
            printf("Marching Cube Cells shader linking failed\n");
            return False;
        }
        printf("shader linking done\n");
        // map uniforms with CPU variables
        scalerFieldSamplerUniform_cubes = glGetUniformLocation(shaderProgramObject_marching_cubes, "scalar_field");
        cellsPerAxisUniform = glGetUniformLocation(shaderProgramObject_marching_cubes, "cells_per_axis");
        isoLevelUniform_cubes = glGetUniformLocation(shaderProgramObject_marching_cubes, "iso_level");

        glUseProgram(shaderProgramObject_marching_cubes);

        glUniform1i(cellsPerAxisUniform, cells_per_axis);
        glUniform1f(isoLevelUniform_cubes, isosurface_level);
        glUniform1i(scalerFieldSamplerUniform_cubes, 1);

        glGenBuffers(1, &marching_cubes_cells_types_buffer_id);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, marching_cubes_cells_types_buffer_id);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, cells_in_3d_space * sizeof(GLint), NULL, GL_STATIC_DRAW);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        glGenTransformFeedbacks(1, &marching_cubes_cells_transform_feedback_object_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);

        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, marching_cubes_cells_types_buffer_id);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* [Stage 3 Creating texture] */
        /* Generate a texture object to hold cell type data. (We will explain why the texture later). */
        glGenTextures(1, &marching_cubes_cells_types_texture_object_id);

        /* Marching cubes cell type data uses GL_TEXTURE_3D target of texture unit 2. */
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_3D, marching_cubes_cells_types_texture_object_id);

        /* Prepare texture storage for marching cube cell type data. */
        glTexStorage3D(GL_TEXTURE_3D, 1, GL_R32I, cells_per_axis, cells_per_axis, cells_per_axis);
        /* [Stage 3 Creating texture] */

        /* Tune texture settings to use it as a data source. */
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return True;
    }

    Bool initMarchingCubeTriangles(void)
    {
        // /** Name of sphere_positions_uniform_block uniform block. */
        // const GLchar *marching_cubes_triangles_uniform_sphere_positions_name = "sphere_positions_uniform_block";

        // code
        GLuint vertexShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubetriangles.vs", VERTEX);

        GLuint fragmentShaderObject = CreateAndCompileShaderObjects("./src/shaders/metaball/marchingcubetriangles.fs", FRAGMENT);

        shaderProgramObject_marching_cube_triangles = glCreateProgram();

        glAttachShader(shaderProgramObject_marching_cube_triangles, vertexShaderObject);
        glAttachShader(shaderProgramObject_marching_cube_triangles, fragmentShaderObject);

        // glBindAttribLocation(shaderProgramObject_marching_cube_triangles, MATRIX_ATTRIBUTE_POSITION, "a_position");
        // glBindAttribLocation(shaderProgramObject_marching_cube_triangles, MATRIX_ATTRIBUTE_TEXTURE0, "a_texcoord");

        Bool bLinkShaderStatus = LinkShaderProgramObject(shaderProgramObject_marching_cube_triangles);
        if (bLinkShaderStatus == False)
        {
            return False;
        }

        // map uniforms with CPU variables
        samplesPerAxisUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "samples_per_axis");
        cellTypesSamplerUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "cell_types");
        scalerFieldSamplerUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "scalar_field");
        triTableSamplerUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "tri_table");
        mvpUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "mvp");
        isoLevelUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "iso_level");
        timeMarchingTriangleUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "time");
        spherePositionIdUniform = glGetUniformLocation(shaderProgramObject_marching_cube_triangles, "sphere_positions_uniform_block");

        // have to pass model * view * projection as a single uniform
        calc_mvp(mvp);

        // initializing uniforms
        glUniform1f(isoLevelUniform, isosurface_level);
        glUniform1i(samplesPerAxisUniform, samples_per_axis);
        glUniform1i(triTableSamplerUniform, 4);
        glUniform1i(cellTypesSamplerUniform, 2);
        glUniform1i(scalerFieldSamplerUniform, 1);
        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE,  perspectiveProjectionMatrix * viewMatrix * modelMatrix  );
        // glUniformMatrix4fv(mvpUniform, 1, GL_FALSE,  mvp.getAsArray());

        /* Allocate memory for buffer */
        glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);

        /* Generate an Id for a texture object to hold look-up array data (tri_table). */
        glGenTextures(1, &marching_cubes_triangles_lookup_table_texture_id);

        /* Lookup array (tri_table) uses GL_TEXTURE_2D target of texture unit 4. */
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, marching_cubes_triangles_lookup_table_texture_id);

        /* Tune texture settings to use it as a data source. */
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        /* Load lookup table (tri_table) into texture. */
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32I, mc_vertices_per_cell, mc_cells_types_count);
        glTexSubImage2D(GL_TEXTURE_2D,        /* Use texture bound to GL_TEXTURE_2D               */
                        0,                    /* Base image level                                 */
                        0,                    /* From the texture origin                          */
                        0,                    /* From the texture origin                          */
                        mc_vertices_per_cell, /* Width will represent vertices in all 5 triangles */
                        mc_cells_types_count, /* Height will represent cell type                  */
                        GL_RED_INTEGER,       /* Texture will have only one component             */
                        GL_INT,               /* ... of type int                                  */
                        tri_table             /* Data will be copied directly from tri_table      */
        );

        /* Generate a vertex array object. We'll go with the explanation later. */
        glGenVertexArrays(1, &marching_cubes_triangles_vao_id);

        /* In OpenGL ES, draw calls require a bound vertex array object.
         * Even though we're not using any per-vertex attribute data, we still need to bind a vertex array object.
         */
        glBindVertexArray(marching_cubes_triangles_vao_id);


        return True;
    }

    void calc_mvp(Matrix& mvp)
    {
        /* Define projection properties. */
        float degreesToRadiansCoefficient = atanf(1) / 45;                            /* Coefficient to recalculate degrees to radians.      */
        float frustum_fovy                = 45.0f;                                    /* 45 degrees field of view in the y direction.        */
        float frustum_aspect              = (float)giWindowWidth/(float)giWindowHeight; /* Aspect ratio.                                       */
        float frustum_z_near              = 0.01f;                                    /* How close the viewer is to the near clipping plane. */
        float frustum_z_far               = 100.0f;                                   /* How far the viewer is from the far clipping plane.  */
        float camera_distance             = 2.5f;                                     /* Distance from camera to scene center.               */

        /* Matrix that stores temporary matrix data for translation transformations. */
        Matrix mat4_translate  = Matrix::createTranslation(-0.5, -0.5, -0.5 -5.0);

        /* Matrix that stores temporary matrix data for scale transformations. */
        Matrix mat4_scale      = Matrix::createScaling    ( 2.0,  2.0,  2.0);

        /* Matrix that transforms the vertices from model space to world space. */
        /* Translate and scale coordinates from [0..1] to [-1..1] range for full visibility. */
        Matrix mat4_model_view = mat4_scale * mat4_translate;

        /* Pull the camera back from the scene center. */
        mat4_model_view[14] -= float(camera_distance);

        /* Create the perspective matrix from frustum parameters. */
        Matrix mat4_perspective = Matrix::matrixPerspective(degreesToRadiansCoefficient * frustum_fovy, frustum_aspect, frustum_z_near, frustum_z_far);

        /* MVP (Model View Perspective) matrix is a result of multiplication of Perspective Matrix by Model View Matrix. */
        mvp = mat4_perspective * mat4_model_view;
    }

    void renderMetaball(void)
    {
        /* Update time. */
        // printf("timer : %f\n", ELAPSED_TIME);
        model_time += 0.01f;
        // printf("timer : %f\n", ELAPSED_TIME);
        calc_mvp(mvp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, spheres_updater_transform_feedback_object_id);
        
        glEnable(GL_RASTERIZER_DISCARD);
        /* [Stage 1 Enable GL_RASTERIZER_DISCARD] */
        {
            
            glUseProgram(shaderProgramObject_sphere_updater);

            glUniform1f(timeUniform, model_time);
            
            glBeginTransformFeedback(GL_POINTS);
            {
                // glDrawArrays(GL_POINTS, 0, n_spheres);
            }
            glEndTransformFeedback();
        }
        /* [Stage 1 Disable GL_RASTERIZER_DISCARD] */
        glDisable(GL_RASTERIZER_DISCARD);
        
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        
        glBindBuffer(GL_UNIFORM_BUFFER, spheres_updater_sphere_positions_buffer_object_id);

        /* Bind buffer object to store calculated scalar field values. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, scalar_field_transform_feedback_object_id);

        /* Shorten GL pipeline: we will use vertex shader only. */
        glEnable(GL_RASTERIZER_DISCARD);
        {
            /* Select program for scalar field generation stage. */
            glUseProgram(shaderProgramObject_scaler_field);

            /* Activate transform feedback mode. */
            glBeginTransformFeedback(GL_POINTS);
            {
                /* Run scalar field calculation for all vertices in space. */
                // glDrawArrays(GL_POINTS, 0, samples_in_3d_space);
            }
            glEndTransformFeedback();
        }
        glDisable(GL_RASTERIZER_DISCARD);

        /* Unbind buffers used at this stage. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        
        /* [Stage 2 Scalar field generation stage move data to texture] */
        glActiveTexture(GL_TEXTURE1);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, scalar_field_buffer_object_id);
        glTexSubImage3D(GL_TEXTURE_3D,    /* Use texture bound to GL_TEXTURE_3D                                     */
                                 0,                /* Base image level                                                       */
                                 0,                /* From the texture origin                                                */
                                 0,                /* From the texture origin                                                */
                                 0,                /* From the texture origin                                                */
                                 samples_per_axis, /* Texture have same width as scalar field in buffer                      */
                                 samples_per_axis, /* Texture have same height as scalar field in buffer                     */
                                 samples_per_axis, /* Texture have same depth as scalar field in buffer                      */
                                 GL_RED,           /* Scalar field gathered in buffer has only one component                 */
                                 GL_FLOAT,         /* Scalar field gathered in buffer is of float type                       */
                                 NULL              /* Scalar field gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target */
                                 );
        /* [Stage 2 Scalar field generation stage move data to texture] */

        /* 3. Marching cube algorithm cell splitting stage.
                 */
        /* Bind buffer to store cell type data. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, marching_cubes_cells_transform_feedback_object_id);

        /* Shorten GL pipeline: we will use vertex shader only. */
        glEnable(GL_RASTERIZER_DISCARD);
        {
            /* Select program for Marching Cubes algorthim's cell splitting stage. */
            glUseProgram(shaderProgramObject_marching_cubes);

            /* Activate transform feedback mode. */
            glBeginTransformFeedback(GL_POINTS);
            {
                /* [Stage 3 Execute vertex shader] */
                /* Run Marching Cubes algorithm cell splitting stage for all cells. */
                // glDrawArrays(GL_POINTS, 0, cells_in_3d_space);
                /* [Stage 3 Execute vertex shader] */
            }
            glEndTransformFeedback();
        }
        glDisable(GL_RASTERIZER_DISCARD);

        /* Unbind buffers used at this stage. */
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        /* Copy data from buffer into texture bound to target GL_TEXTURE2 in texture unit 2.
         * We need to move this data to a texture object, as there is no way we could access data
         * stored within a buffer object in a OpenGL ES 3.0 shader.
         */
        glActiveTexture(GL_TEXTURE2);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, marching_cubes_cells_types_buffer_id);
        glTexSubImage3D(GL_TEXTURE_3D,  /* Use texture bound to GL_TEXTURE_3D                                   */
                                 0,              /* Base image level                                                     */
                                 0,              /* From the texture origin                                              */
                                 0,              /* From the texture origin                                              */
                                 0,              /* From the texture origin                                              */
                                 cells_per_axis, /* Texture have same width as cells by width in buffer                  */
                                 cells_per_axis, /* Texture have same height as cells by height in buffer                */
                                 cells_per_axis, /* Texture have same depth as cells by depth in buffer                  */
                                 GL_RED_INTEGER, /* Cell types gathered in buffer have only one component                */
                                 GL_INT,         /* Cell types gathered in buffer are of int type                        */
                                 NULL            /* Cell types gathered in buffer bound to GL_PIXEL_UNPACK_BUFFER target */
                                 );

        /* 4. Marching Cubes algorithm triangle generation stage.
         *
         * At this stage, we render exactly (3 vertices * 5 triangles per cell *
         * amount of cells the scalar field is split to) triangle vertices.
         * Then render triangularized geometry.
         */
        glActiveTexture(GL_TEXTURE0);

        /* Activate triangle generating and rendering program. */
        // glUseProgram(shaderProgramObject_marching_cube_triangles);

        /* Specify input arguments to vertex shader. */
        glUniform1f(timeMarchingTriangleUniform, model_time);
        // glUniformMatrix4fv(mvpUniform, 1, GL_FALSE,  mvp.getAsArray());
        // glUniformMatrix4fv(mvpUniform, 1, GL_FALSE,  perspectiveProjectionMatrix * viewMatrix * modelMatrix  );

        /* [Stage 4 Run triangle generating and rendering program] */
        /* Run triangle generating and rendering program. */
        glDrawArrays(GL_TRIANGLES, 0, cells_in_3d_space * triangles_per_cell * vertices_per_triangle);
        /* [Stage 4 Run triangle generating and rendering program] */
    }

    void updateMetabsll(void)
    {
        model_time = model_time + 0.01f;
        if (model_time > 1000.0f)
            model_time = 0.0f;
    }

    void uninitializeFire(void)
    {
        
        if (vbo_texcoord_fire)
        {
            glDeleteBuffers(1, &vbo_texcoord_fire);
            vbo_texcoord_fire = 0;
        }

        if (vbo_position_fire)
        {
            glDeleteBuffers(1, &vbo_position_fire);
            vbo_position_fire = 0;
        }

        if (vao_fire)
        {
            glDeleteVertexArrays(1, &vao_fire);
            vao_fire = 0;
        }

        UninitializeShaders(shaderProgramObject_sphere_updater);
    }
};
