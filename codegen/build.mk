define download_file
URL="$(1)" SAVE_AS="$(2)" ruby scripts/download_file.rb
endef

CODEGEN_EXE = codegen/codegen$(EXE)
$(CODEGEN_EXE):
	$(call download_file, https://github.com/lib-ruby-parser/lib-ruby-parser/releases/download/codegen-v1.0.1/codegen-$(TARGET)$(EXE), $@)
	chmod +x $(CODEGEN_EXE)
CLEAN += $(CODEGEN_EXE)

# Codegen deps
src/node_to_v8.cpp: codegen/node_to_v8.cpp.liquid $(CODEGEN_EXE)
	$(CODEGEN_EXE) --template $< --write-to $@
CLEAN += src/node_to_v8.cpp
CODEGEN_FILES += src/node_to_v8.cpp

src/message_to_v8.cpp: codegen/message_to_v8.cpp.liquid $(CODEGEN_EXE)
	$(CODEGEN_EXE) --template $< --write-to $@
CLEAN += src/message_to_v8.cpp
CODEGEN_FILES += src/message_to_v8.cpp

types.d.ts: codegen/types.d.ts.liquid $(CODEGEN_EXE)
	$(CODEGEN_EXE) --template $< --write-to $@
CLEAN += types.d.ts
CODEGEN_FILES += types.d.ts

pkg/nodes.js: codegen/nodes.js.liquid $(CODEGEN_EXE)
	$(CODEGEN_EXE) --template $< --write-to $@
CLEAN += pkg/nodes.js
CODEGEN_FILES += pkg/nodes.js

pkg/messages.js: codegen/messages.js.liquid $(CODEGEN_EXE)
	$(CODEGEN_EXE) --template $< --write-to $@
CLEAN += pkg/messages.js
CODEGEN_FILES += pkg/messages.js

pkg/tokens.js: codegen/tokens.js.liquid $(CODEGEN_EXE)
	$(CODEGEN_EXE) --template $< --write-to $@
CLEAN += pkg/tokens.js
CODEGEN_FILES += pkg/tokens.js

do-codegen: $(CODEGEN_FILES)
.PHONY: do-codegen
