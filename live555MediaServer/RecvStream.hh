#ifndef __RECOFACE_H__
#define __RECOFACE_H__

typedef long long int64_t;

class CRecoFace
{
public:
	static int64_t read_from_bs(void *fFid, unsigned char* fTo, int64_t fMaxSize);
	static int vedio_init(void);
	static void vedio_release(void);
};

#endif /*__RECOFACE_H__*/
