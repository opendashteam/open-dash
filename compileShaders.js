import { execSync } from 'node:child_process';
import fs from 'fs';
import path from 'path';

const __dirname = import.meta.dirname;

const sdlShaderCrossPath = "C:/Program Files (x86)/SDL3_shadercross/bin/shadercross.exe";

function run(cmd) {
    // console.log("> ", cmd);
    try {
        execSync(cmd, { stdio: 'inherit' });
    } catch {}
}

function compileShader(shaderPath) {
    let type = null;
    if (shaderPath.endsWith(".vert"))
        type = "vert";
    if (shaderPath.endsWith(".frag"))
        type = "frag";

    if (type == null)
        return;

    console.log(`Compiling ${shaderPath}`);

    let spvDst  = path.join(__dirname, `assets/shaders/spv/${path.basename(shaderPath)}.spv`);
    let dxilDst = path.join(__dirname, `assets/shaders/dxil/${path.basename(shaderPath)}.dxil`);
    let mslDst  = path.join(__dirname, `assets/shaders/msl/${path.basename(shaderPath)}.msl`);

    if (type == "vert")
        run(`glslc -DVERTEX_SHADER -fshader-stage=vert ${shaderPath} -o ${spvDst}`);
    else
        run(`glslc -DFRAGMENT_SHADER -fshader-stage=frag ${shaderPath} -o ${spvDst}`);

    run(`"${sdlShaderCrossPath}" ${spvDst} -o ${dxilDst} -s SPIRV -d DXIL`);
    run(`"${sdlShaderCrossPath}" ${spvDst} -o ${mslDst} -s SPIRV -d MSL`);
}

fs.mkdirSync(path.join(__dirname, "assets/shaders/spv"), {recursive: true});
fs.mkdirSync(path.join(__dirname, "assets/shaders/dxil"), {recursive: true});
fs.mkdirSync(path.join(__dirname, "assets/shaders/msl"), {recursive: true});

for (const name of fs.readdirSync(path.join(__dirname, "assets/shaders"))) {
    const fullPath = path.join(__dirname, "assets/shaders", name);
    compileShader(fullPath);
}