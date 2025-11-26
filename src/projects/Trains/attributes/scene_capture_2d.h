#ifndef PROJECTS_TRAINS_ATTRIBUTES_SCENE_CAPTURE_2D_H
#define PROJECTS_TRAINS_ATTRIBUTES_SCENE_CAPTURE_2D_H

#include <GL/glew.h>

#include "core/ecs/types.h"

struct SceneCapture2D : core::ecs::IAttribute {
	int texture_width;
	int texture_height;
	GLuint texture_id;

	SceneCapture2D()
	    : texture_width(512),
	      texture_height(512),
	      texture_id(0) {}
};

#endif // PROJECTS_TRAINS_ATTRIBUTES_SCENE_CAPTURE_2D_H
