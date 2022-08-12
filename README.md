# Mowve-It
OCR Creative iMedia Final Project Title (2015).

'Lawn Mowing' themed mini video game (written in C++, targetting Windows/DirectX 9.0).

-----

Throwback to school (another timecapsule) and eyesore reminder of an unrefined, immature coding style.

This project was the final constituent of the end-of-year coursework submission prior to receiving the OCR award (and received full marks).

-----

I distinctly recall during the production, the epochal discovery (for the first time) of 'undefined behaviour' and what it was exactly, because it repeatedly blew the application up: the volatility of virtual class objects that are manipulated by 'memset' (or 'ZeroMemory'), which is a function whose use is heavily encouraged by C programmers for 'saving cycles', but has the side effect (at least on Windows C++ implementations) of overwriting the sensitive Virtual Function Table Pointer (vfptr/vftbl/vftp) which is usually a hidden member of objects deriving virtually from base classes and who inherit and override virtual methods. It (for me) illuminated the purpose of the class constructor when ordinarily a (faster) zero initialisation (through memset()) seemed more attractive against (otherwise the) explicit (zero-)initialisation of each individual object member (which can quickly feel like boilerplate). It was this new knowledge that propelled me to master assembly and C, in order to gain foundational insight into how C++ (and its intricacies) really operated under the hood.
