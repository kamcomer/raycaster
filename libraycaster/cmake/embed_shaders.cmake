function(escape_file VAR FILE)
  file(READ "${FILE}" CONTENT)
  string(REPLACE "\\" "\\\\" CONTENT "${CONTENT}")
  string(REPLACE "\"" "\\\"" CONTENT "${CONTENT}")
  string(REPLACE "\n" "\\n" CONTENT "${CONTENT}")
  set(${VAR} "${CONTENT}" PARENT_SCOPE)
endfunction()

escape_file(WALL   "${SHADER_DIR}/wall.hlsl")
escape_file(FLOOR  "${SHADER_DIR}/floor.hlsl")
escape_file(SPRITE "${SHADER_DIR}/sprite.hlsl")

file(WRITE "${OUTPUT_DIR}/shaders_embed.h"
  "#pragma once\n"
  "extern const char g_wall_hlsl[];\n"
  "extern const char g_floor_hlsl[];\n"
  "extern const char g_sprite_hlsl[];\n"
)

file(WRITE "${OUTPUT_DIR}/shaders_embed.c"
  "#include \"shaders_embed.h\"\n"
  "const char g_wall_hlsl[] = \"${WALL}\";\n"
  "const char g_floor_hlsl[] = \"${FLOOR}\";\n"
  "const char g_sprite_hlsl[] = \"${SPRITE}\";\n"
)
