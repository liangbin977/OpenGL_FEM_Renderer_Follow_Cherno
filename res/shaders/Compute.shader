#shader compute
#version 430 core

// 声明工作组大小 (必须匹配 C++ 的 512/8)
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// 声明输出图像 (必须匹配 C++ 的 BindImage(0))
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform float u_Time;

void main() {
    // 获取当前像素坐标
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    // 计算颜色 (简单的时间动画)
    vec4 color = vec4(
        0.5 + 0.5 * sin(u_Time + pixel_coords.x * 0.1), 
        0.5 + 0.5 * cos(u_Time + pixel_coords.y * 0.1), 
        0.5, 
        1.0
    );

    // 写入像素
    imageStore(img_output, pixel_coords, color);
}