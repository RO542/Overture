
#ifndef TAG_EXTACTOR_H
#define TAG_EXTRACTOR_H

#include <iostream>
#include <taglib/tpropertymap.h>
#include <taglib/tstringlist.h>
#include <taglib/tvariant.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>





#include"../deps/stb/stb_image.h"
#include"../deps/stb/stb_image_resize2.h"
#include"../deps/stb/stb_image_write.h"
#include"../deps/xxhash.h"



const std::string img_cache_dir = "C:/Users/richa/Code_Projects/prototype-music-player/test_images/";

void downsizeThumbnailAndSave(const TagLib::ByteVector& inputData);


#endif // TAG_EXTRACTOR_H
