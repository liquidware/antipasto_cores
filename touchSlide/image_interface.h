#ifndef __IMAGE_INTERFACE_H_
#define __IMAGE_INTERFACE_H_

#ifdef __cplusplus
extern "C"{
#endif

#define IMAGE_INTERFACE_STORE	'S'
#define IMAGE_INTERFACE_READ	'R'
#define IMAGE_INTERFACE_INFO	'I'
#define IMAGE_INTERFACE_PAGE_DONE	'D'
#define IMAGE_INTERFACE_EXIT	'E'

void image_interface_begin();


#ifdef __cplusplus
} // extern "C"
#endif

#endif



