struct VSInput {
    float2 inPosition;
    float3 inColor;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};
ConstantBuffer<UniformBuffer> ubo;

struct VSOutput {
    float4 pos : SV_Position;
    float3 color;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.pos = mul(ubo.proj, mul(ubo.view, mul(ubo.model, float4(input.inPosition, 0.0, 1.0))));
    output.color = input.inColor;
    return output;
}

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_TARGET {
    // float offset = 500;
    // float scale = 2;
    // float blur = 1;
    // float newX  = (vertIn.pos.x - offset) / scale;
    // float newY  = (vertIn.pos.y - offset) / scale;
    //
    // float output = sin(newX) + sin(newY);
    //
    // output = output / blur;
    // float4 fullOut = float4(output, output, output, 1.0);
    // return fullOut;
    return float4(vertIn.color, 1.0);
}
