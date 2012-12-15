#pragma once


#define _RELEASE_IF(p) { if(p){ (p)->Release(); (p) = nullptr; } }
#define _RELEASE(p) { (p)->Release(); (p) = nullptr; }

#define _DELETE_IF(p) { if(p){ delete (p); (p) = nullptr; } }
#define _DELETE(p) { delete (p); (p) = nullptr; }

#define _RELEASE_AND_DELETE_IF(p) { if(p){ (p)->Release(); delete (p); (p) = nullptr; } }
#define _RELEASE_AND_DELETE(p) { (p)->Release(); delete (p); (p) = nullptr; }
