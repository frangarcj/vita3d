PHONY := all package clean
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

CC := arm-vita-eabi-gcc
CXX := arm-vita-eabi-g++
STRIP := arm-vita-eabi-strip

PROJECT_TITLE := Vita3d
PROJECT_TITLEID := FFFF00002

PROJECT := Vita3d
CXXFLAGS += -std=c++11 -Isrc/include -Iinclude

LIBS := -lvita2d -lSceKernel_stub -lSceDisplay_stub -lSceGxm_stub \
	-lSceSysmodule_stub -lSceCtrl_stub -lScePgf_stub \
	-lSceCommonDialog_stub -lSceNet_stub -lSceNetCtl_stub -lSceHttp_stub -lfreetype -lpng -ljpeg -lz -lm -lc -L./libs -lbullet -ldebugnet

SRC_C :=$(call rwildcard, src/, *.c)
SRC_CPP :=$(call rwildcard, src/, *.cpp)
SRC_GXP :=$(call rwildcard, shaders/, *.gxp)

OBJ_DIRS := $(addprefix out/, $(dir $(SRC_C:src/%.c=%.o))) $(addprefix out/, $(dir $(SRC_CPP:src/%.cpp=%.o))) $(addprefix out/, $(dir $(SRC_GXP:shaders/%.gxp=%.o)))
OBJS := $(addprefix out/, $(SRC_C:src/%.c=%.o)) $(addprefix out/, $(SRC_CPP:src/%.cpp=%.o)) $(addprefix out/, $(SRC_GXP:shaders/%.gxp=%.o))

all: package

package: $(PROJECT).vpk

$(PROJECT).vpk: eboot.bin param.sfo
	vita-pack-vpk -s param.sfo -b eboot.bin \
		--add sce_sys/icon0.png=sce_sys/icon0.png \
		--add sce_sys/livearea/contents/bg.png=sce_sys/livearea/contents/bg.png \
		--add sce_sys/livearea/contents/startup.png=sce_sys/livearea/contents/startup.png \
		--add sce_sys/livearea/contents/template.xml=sce_sys/livearea/contents/template.xml \
		--add res/Shaders/basic_f.gxp=res/Shaders/basic_f.gxp \
		--add res/Shaders/basic_v.gxp=res/Shaders/basic_v.gxp \
		--add res/Shaders/light_f.gxp=res/Shaders/light_f.gxp \
		--add res/Shaders/light_v.gxp=res/Shaders/light_v.gxp \
		--add res/Shaders/clear_f.gxp=res/Shaders/clear_f.gxp \
		--add res/Shaders/clear_v.gxp=res/Shaders/clear_v.gxp \
		--add res/Mesh/car/chassis.obj=res/Mesh/car/chassis.obj \
		--add res/Mesh/car/wheel.obj=res/Mesh/car/wheel.obj \
		--add res/Mesh/car/chassis.mtl=res/Mesh/car/chassis.mtl \
		--add res/Mesh/car/wheel.mtl=res/Mesh/car/wheel.mtl \
		--add res/Mesh/car/chassis_uv.png=res/Mesh/car/chassis_uv.png \
		--add res/Mesh/car/wheel_uv.png=res/Mesh/car/wheel_uv.png \
		--add res/Mesh/Ground/ground.obj=res/Mesh/Ground/ground.obj \
		--add res/Mesh/Ground/ground.mtl=res/Mesh/Ground/ground.mtl \
		--add res/Mesh/Ground/texture.png=res/Mesh/Ground/texture.png \
$(PROJECT).vpk

eboot.bin: $(PROJECT).velf
	vita-make-fself $(PROJECT).velf eboot.bin

param.sfo:
	vita-mksfoex -s TITLE_ID="$(PROJECT_TITLEID)" "$(PROJECT_TITLE)" param.sfo

$(PROJECT).velf: $(PROJECT).elf
	$(STRIP) -g $<
	vita-elf-create $< $@

$(PROJECT).elf: $(OBJS)
	$(CXX)  -Wl,-q -o $@ $^ $(LIBS)

$(OBJ_DIRS):
	mkdir -p $@

out/%.o : shaders/%.gxp | $(OBJ_DIRS)
	arm-vita-eabi-ld -r -b binary -o $@ $^

out/%.o : src/%.cpp | $(OBJ_DIRS)
	arm-vita-eabi-g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(PROJECT).velf $(PROJECT).elf $(PROJECT).vpk param.sfo eboot.bin $(OBJS)
	rm -rf $(abspath $(OBJ_DIRS))

install:
	curl -T Vita3d.vpk ftp://192.168.1.5:1337/ux0:/

install_u:
	curl -T eboot.bin ftp://192.168.1.5:1337/ux0:app/FFFF00002/
