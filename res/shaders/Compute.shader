#shader compute
#version 430 core

// 声明工作组大小 (必须匹配 C++ 的 512/8)
layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

// 声明输出图像 (必须匹配 C++ 的 BindImage(0))
layout(rgba32f, binding = 0) uniform image2D img_Input;
layout(rgba32f, binding = 1) uniform image2D img_Output;

uniform vec2 U_MousePos; // 鼠标位置 (0-1 范围)
uniform bool U_MousePressed; // 鼠标是否按下
//每一个线程负责计算一个像素的颜色值
void main() {
    // 获取当前像素坐标索引
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    // 获取图像尺寸
    ivec2 image_size = imageSize(img_Input);

    vec4 location = imageLoad(img_Input, pixel_coords); // 从输入图像读取当前像素颜色
    float h = location.r; // 取红色通道作为高度值
    float v = location.g; // 取绿色通道作为速度值
    
    if(U_MousePressed){
        // 计算鼠标对当前像素的影响
        vec2 pixel_center =  (vec2(pixel_coords) + 0.5) / vec2(image_size); // 像素中心位置 (0-1 范围)
        float dist = distance(pixel_center, U_MousePos); // 计算像素中心与鼠标位置的距离
        float influence = exp(-dist * dist * 100.0); // 距离越近影响越大 (高斯函数)
        v += influence * 0.1; // 增加速度值，推动水流
    }
    float h_left = imageLoad(img_Input, pixel_coords + ivec2(-1, 0)).r; // 左邻居的高度值
    float h_right = imageLoad(img_Input, pixel_coords + ivec2(1, 0)).r; // 右邻居的高度值
    float h_up = imageLoad(img_Input, pixel_coords + ivec2(0, -1)).r; // 上邻居的高度值
    float h_down = imageLoad(img_Input, pixel_coords + ivec2(0, 1)).r; // 下邻居的高度值

    float laplacian = h_left + h_right + h_up + h_down - 4.0 * h; // 计算拉普拉斯算子
    float k = 0.1; // 波速常数
    float damping = 0.99; // 阻尼系数
    v += k * laplacian; // 根据拉普拉斯算子更新速度
    v *= damping; // 应用阻尼
    h += v; // 根据速度更新高度值

    imageStore(img_Output, ivec2(pixel_coords), vec4(h, v, 0.0, 1.0)); // 将新的高度和速度值写入输出图像
    
}