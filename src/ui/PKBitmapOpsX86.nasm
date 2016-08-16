;
; Copyright (c) 2004 - Damien Chavarria
; 

section .text
align 4

%imacro cglobalfunc 1
%ifdef __PIC__
global %1:function
%else
%ifdef  __NOU__
global %1
%else
global _%1
%define %1 _%1
%endif
%endif
%endmacro

cglobalfunc _alphaBlitMMXNOGlobalAlpha

;;
;;	void _alphaBlitMMXNOGlobalAlpha(unsigned char *src,		[ebp+8]
;;									unsigned char *dst,		[ebp+12] 
;;									unsigned int  w,		[ebp+16] 
;;									unsigned int  h,		[ebp+20] 
;;									unsigned int  s_line,	[ebp+24] 
;;									unsigned int  d_line)	[ebp+28]
;;

_alphaBlitMMXNOGlobalAlpha:
        
		;; function start

		push      ebp
        mov       ebp, esp

		; parameters start at [ebp+8]

		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx
		
		;; setup source, dest 
		;; and height counter

		mov  esi, [ebp+8] ; src
		mov  edi, [ebp+12] ; dst
		mov  ecx, [ebp+20] ; h

		pxor mm7, mm7

AArunloopy:

		mov  ebx, [ebp+16] ; w

AArunloopx: 
		
		;; extract the source 
		;; pixel alpha 

		mov  eax, [esi]
		movd mm0, eax

		shr  eax, 24 ;; alpha value is the 
					 ;; high-order byte...

		;; prepare unpacked mm2 & mm3
		;; mmx registers with alpha
		;; and (256 - alpha) values

		movd mm2, eax
		punpcklwd mm2, mm2
		punpcklwd mm2, mm2

		mov edx, 0x00000100 ;; 256
		sub edx, eax

		movd mm3, edx
		punpcklwd mm3, mm3
		punpcklwd mm3, mm3

		;; get and unpack

		movd mm1, [edi]

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7

		;; combine

		pmullw mm0, mm2
		pmullw mm1, mm3

		paddw  mm1, mm0
		psrlw  mm1, 8

		;; pack and copy back

		packuswb  mm1, mm7
		movd [edi], mm1

		;; increment pointers

		add edi, 4
		add esi, 4

		dec ebx
		jnz AArunloopx

		;; add line remainders

		add esi, [ebp+24]
		add edi, [ebp+28]

		dec ecx
		jnz AArunloopy

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;
;;

cglobalfunc _alphaBlitMMXGlobalAlpha


;;
;;	void _alphaBlitMMXGlobalAlpha(unsigned char *src,		[ebp+8]
;;								  unsigned char *dst,		[ebp+12]
;;								  unsigned int  w,			[ebp+16]
;;								  unsigned int  h,			[ebp+20]
;;								  unsigned int  s_line,		[ebp+24]
;;								  unsigned int  d_line,		[ebp+28]
;;								  unsigned int  alpha)		[ebp+32]
;;

_alphaBlitMMXGlobalAlpha:

		;; function start

		push      ebp
        mov       ebp, esp

		; parameters start at [ebp+8]
		
		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx
		
		;; setup source, dest 
		;; and height counter

		mov  esi, [ebp+8] ; src
		mov  edi, [ebp+12] ; dst
		mov  ecx, [ebp+20] ; h

		pxor mm7, mm7

AGrunloopy:

		mov  ebx, [ebp+16] ; w
AGrunloopx: 
		
		;; extract the source 
		;; pixel alpha 

		mov  eax, [esi]
		movd mm0, eax

		shr  eax, 24 ;; alpha value is the 
					 ;; high-order byte...

		;; combine with global alpha
	
		mov edx, [ebp+32] ; alp

		mul edx	   ;; multiplies al, stores in ax
		shr eax, 8 ;; >> 8 to divide by 256

		;; prepare unpacked mm2 & mm3
		;; mmx registers with alpha
		;; and (256 - alpha) values

		movd mm2, eax
		punpcklwd mm2, mm2
		punpcklwd mm2, mm2

		mov edx, 0x00000100 ;; 256
		sub edx, eax

		movd mm3, edx
		punpcklwd mm3, mm3
		punpcklwd mm3, mm3

		;; get and unpack

		movd mm1, [edi]

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7

		;; combine

		pmullw mm0, mm2
		pmullw mm1, mm3

		psrlw  mm0, 8
		psrlw  mm1, 8

		paddw  mm1, mm0

		;; pack and copy back

		packuswb  mm1, mm7
		movd [edi], mm1

		;; increment pointers

		add edi, 4
		add esi, 4

		dec ebx
		jnz AGrunloopx

		;; add line remainders

		add esi, [ebp+24] ; s_line
		add edi, [ebp+28] ; d_line

		dec ecx
		jnz AGrunloopy

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;

cglobalfunc _alphaBlitSSENOGlobalAlpha

;;
;;	void _alphaBlitSSENOGlobalAlpha(unsigned char *src,		[ebp+8]
;;									unsigned char *dst,		[ebp+12] 
;;									unsigned int  w,		[ebp+16] 
;;									unsigned int  h,		[ebp+20] 
;;									unsigned int  s_line,	[ebp+24] 
;;									unsigned int  d_line)	[ebp+28]
;;

_alphaBlitSSENOGlobalAlpha:
        
		;; function start

		push      ebp
        mov       ebp, esp

		; parameters start at [ebp+8]

		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx
		
		;; setup source, dest 
		;; and height counter

		mov  esi, [ebp+8] ; src
		mov  edi, [ebp+12] ; dst
		mov  ecx, [ebp+20] ; h

		pxor mm7, mm7

		mov   eax, 0x01000100
		movd  mm5, eax
		psllq mm5, 32		  ;; shift it to upper dword
		movd  mm6, eax
		paddd mm5, mm6		  ;; mm5 has unpacked 256 values 

AASrunloopy:

		mov  ebx, [ebp+16] ; w

AASrunloopx: 
		
		;; get and unpack

		movd  mm0, [esi]
		punpcklbw mm0, mm7

		movd mm1, [edi]
		punpcklbw mm1, mm7

		;; extract the source 
		;; pixel alpha 

		pshufw mm2, mm0, 11111111b

		movq  mm3, mm5
		psubw mm3, mm2

		;; combine

		pmullw mm0, mm2
		pmullw mm1, mm3

		paddw  mm1, mm0
		psrlw  mm1, 8

		;; pack and copy back

		packuswb  mm1, mm7
		movd [edi], mm1

		;; increment pointers

		add edi, 4
		add esi, 4

		dec ebx
		jnz AASrunloopx

		;; add line remainders

		add esi, [ebp+24]
		add edi, [ebp+28]

		dec ecx
		jnz AASrunloopy

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;
;;

cglobalfunc _alphaBlitSSEGlobalAlpha


;;
;;	void _alphaBlitSSEGlobalAlpha(unsigned char *src,		[ebp+8]
;;								  unsigned char *dst,		[ebp+12]
;;								  unsigned int  w,			[ebp+16]
;;								  unsigned int  h,			[ebp+20]
;;								  unsigned int  s_line,		[ebp+24]
;;								  unsigned int  d_line,		[ebp+28]
;;								  unsigned int  alpha)		[ebp+32]
;;

_alphaBlitSSEGlobalAlpha:

		;; function start

		push      ebp
        mov       ebp, esp

		; parameters start at [ebp+8]
		
		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx
		
		;; setup source, dest 
		;; and height counter

		mov  esi, [ebp+8] ; src
		mov  edi, [ebp+12] ; dst
		mov  ecx, [ebp+20] ; h

		pxor mm7, mm7

		mov   eax, 0x01000100
		movd  mm5, eax
		psllq mm5, 32		  ;; shift it to upper dword
		movd  mm6, eax
		paddd mm5, mm6		  ;; mm5 has unpacked 256 values 

		;; store global alpha in mm6

		mov  eax, [ebp+32] ; alp
		movd mm6, eax
		punpcklwd mm6, mm6
		punpcklwd mm6, mm6

AGSrunloopy:

		mov  ebx, [ebp+16] ; w
AGSrunloopx: 
		
		movd  mm0, [esi]
		punpcklbw mm0, mm7

		movd mm1, [edi]
		punpcklbw mm1, mm7

		;; extract the source 
		;; pixel alpha 

		pshufw mm2, mm0, 11111111b

		;; combine with global alpha

		pmullw mm2, mm6
		psrlw  mm2, 8

		movq  mm3, mm5
		psubw mm3, mm2

		;; combine

		pmullw mm0, mm2
		pmullw mm1, mm3

		psrlw  mm0, 8
		psrlw  mm1, 8

		paddw  mm1, mm0

		;; pack and copy back

		packuswb  mm1, mm7
		movd [edi], mm1

		;; increment pointers

		add edi, 4
		add esi, 4

		dec ebx
		jnz AGSrunloopx

		;; add line remainders

		add esi, [ebp+24] ; s_line
		add edi, [ebp+28] ; d_line

		dec ecx
		jnz AGSrunloopy

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;

cglobalfunc _blurMMX

;;
;; void _blurMMX(unsigned char *dst, [ebp+8]
;;				 unsigned int  w,	 [ebp+12]
;;				 unsigned int  h);	 [ebp+16]
;;

_blurMMX:

		;; function start

		push      ebp
        mov       ebp, esp

		;; parameters start at [ebp+8]

		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx

		;; setup source = dest 
		;; and width*height counter

		mov  esi, [ebp+8]  ; dst
		mov  edi, [ebp+8]  ; dst
		mov  ebx, [ebp+16] ; h
		mov  ecx, [ebp+12] ; w

		pxor mm7, mm7
		
		;; do first pixel ;;
		;;;;;;;;;;;;;;;;;;;;
		
		;; get values

		movd mm0, [edi]
		movd mm1, [edi + 4]
		movd mm2, [edi + 4*ecx]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7

		;; average

		psllw mm0, 1
		paddw mm0, mm1
		paddw mm0, mm2
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4

		;; do the first line ;;
		;;;;;;;;;;;;;;;;;;;;;;;

		mov  eax, ecx ; w
		sub  eax, 2

blurLine1Loop:

		;; get values

		movd mm0, [edi]
		movd mm1, [edi + 4]
		movd mm2, [edi + 4*ecx]
		movd mm3, [edi - 4]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7
		punpcklbw mm3, mm7

		;; average

		paddw mm0, mm1
		paddw mm0, mm2
		paddw mm0, mm3
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4
		dec eax
		jnz blurLine1Loop
		
		;; Do last pixel of first line ;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
		;; get values

		movd mm0, [edi]
		movd mm1, [edi - 4]
		movd mm2, [edi + 4*ecx]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7

		;; average

		psllw mm0, 1
		paddw mm0, mm1
		paddw mm0, mm2
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4

		;; Do the rest of the lines ;;				
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		dec ebx ;; first line done
		dec ebx ;; want to skip last

blurloopY:

		mov  eax, ecx ; w

		;; Do first pixel of this line ;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		;; get values

		movd mm0, [edi]
		movd mm1, [edi + 4]
		movd mm2, [edi + 4*ecx]
		movd mm3, [esi] ;; esi points one line before edi

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7
		punpcklbw mm3, mm7

		;; average

		paddw mm0, mm1
		paddw mm0, mm2
		paddw mm0, mm3
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4
		add esi, 4

		;; Do the rest of the line ;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		dec eax
		dec eax

blurloopX:

		;; get pixels

		movd mm0, [edi]
		movd mm1, [edi + 4]
		movd mm2, [edi - 4]
		movd mm3, [edi + 4*ecx]
		movd mm4, [esi]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7
		punpcklbw mm3, mm7
		punpcklbw mm4, mm7

		;; average

		psllw mm0, 2
		paddw mm0, mm1
		paddw mm0, mm2
		paddw mm0, mm3
		paddw mm0, mm4
		psrlw mm0, 3

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		;; done

		add edi, 4
		add esi, 4
		
		dec eax
		jnz blurloopX		

		;; Do last pixel of this line ;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		;; get values

		movd mm0, [edi]
		movd mm1, [edi - 4]
		movd mm2, [edi + 4*ecx]
		movd mm3, [esi]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7
		punpcklbw mm3, mm7

		;; average

		paddw mm0, mm1
		paddw mm0, mm2
		paddw mm0, mm3
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4
		add esi, 4

		dec ebx
		jnz blurloopY		

		;; Do last line ;;
		;;;;;;;;;;;;;;;;;;

		;; do first pixel ;;
		;;;;;;;;;;;;;;;;;;;;
		
		;; get values

		movd mm0, [edi]
		movd mm1, [edi + 4]
		movd mm2, [esi]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7

		;; average

		psllw mm0, 1
		paddw mm0, mm1
		paddw mm0, mm2
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4
		add esi, 4

		;; Do rest of the last line ;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		mov  eax, ecx ; w
		sub  eax, 2

blurLastLineLoop:

		;; get values

		movd mm0, [edi]
		movd mm1, [edi + 4]
		movd mm2, [esi]
		movd mm3, [edi - 4]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7
		punpcklbw mm3, mm7

		;; average

		paddw mm0, mm1
		paddw mm0, mm2
		paddw mm0, mm3
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		add edi, 4
		add esi, 4

		dec eax
		jnz blurLastLineLoop
		
		;; Do last pixel of first line ;;
		;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		
		;; get values

		movd mm0, [edi]
		movd mm1, [edi - 4]
		movd mm2, [esi]

		;; unpack

		punpcklbw mm0, mm7
		punpcklbw mm1, mm7
		punpcklbw mm2, mm7

		;; average

		psllw mm0, 1
		paddw mm0, mm1
		paddw mm0, mm2
		psrlw mm0, 2

		;; pack

		packuswb mm0, mm7

		;; copy back

		movd [edi], mm0

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;

cglobalfunc _gradientLineMMX

;;
;; void _gradientLineMMX(unsigned int *dst, [ebp+8]
;; 						 unsigned int  w,	[ebp+12]
;;						 unsigned char r1,  [ebp+16]
;;					     unsigned char g1,  [ebp+20]
;;					     unsigned char b1,  [ebp+24]
;;					     unsigned char r2,  [ebp+28]
;;					     unsigned char g2,  [ebp+32]
;;					     unsigned char b2); [ebp+36]
;;

_gradientLineMMX:

		;; function start

		push      ebp
        mov       ebp, esp

		;; parameters start at [ebp+8]

		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx

		;; setup

		mov ah, 0xFF     ; alpha
		mov al, [ebp+16] ; r1
		shl eax, 16

		mov ah, [ebp+20] ; g1
		mov al, [ebp+24] ; b1

		movd mm0, eax ; mm0 contains color 1

		mov ah, 0xFF     ; alpha
		mov al, [ebp+28] ; r2
		shl eax, 16

		mov ah, [ebp+32] ; g2
		mov al, [ebp+36] ; b2

		movd mm1, eax ; mm1 contains color 1

		pxor mm7, mm7
		punpcklbw mm0, mm7
		punpcklbw mm1, mm7

		;; prepare to fill the line buffer

		mov ebx, [ebp+12] ; w
		mov ecx, [ebp+12] ; w copy
		mov edi, [ebp+8]  ; dst

gradientLoop:

		;; calculate alpha

		xor edx, edx

		mov eax, ebx
		shl eax, 8 ; * 256
		div	ecx

		;; eax contains alpha

		movd mm2, eax
		punpcklwd mm2, mm2
		punpcklwd mm2, mm2

		mov edx, 0x00000100 ;; 256
		sub edx, eax

		movd mm3, edx
		punpcklwd mm3, mm3
		punpcklwd mm3, mm3

		;; do color blending

		movq mm4, mm0 ; color 1
		movq mm5, mm1 ; color 2

		pmullw mm4, mm2
		pmullw mm5, mm3

		paddw  mm4, mm5
		psrlw  mm4, 8

		;; pack and copy back

		packuswb  mm4, mm7
		movd [edi], mm4

		;; increment

		add edi, 4

		dec ebx;
		jnz gradientLoop

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret
	

cglobalfunc _gradientColorMMX

;;
;; void _gradientColorMMX(unsigned int *dst,	[ebp+8]
;;					      unsigned int  index,	[ebp+12]
;;					      unsigned int  width,	[ebp+16]
;;					      unsigned char r1,		[ebp+20]
;;					      unsigned char g1,		[ebp+24]
;;						  unsigned char b1,		[ebp+28]
;;						  unsigned char r2,		[ebp+32]
;;						  unsigned char g2,		[ebp+36]
;;					      unsigned char b2);	[ebp+40]
;;

_gradientColorMMX:

		;; function start

		push      ebp
        mov       ebp, esp

		;; parameters start at [ebp+8]

		push edi
		push esi
		push eax
		push ebx
		push ecx
		push edx

		;; setup

		mov ah, 0xFF     ; alpha
		mov al, [ebp+20] ; r1
		shl eax, 16

		mov ah, [ebp+24] ; g1
		mov al, [ebp+28] ; b1

		movd mm0, eax ; mm0 contains color 1

		mov ah, 0xFF     ; alpha
		mov al, [ebp+32] ; r2
		shl eax, 16

		mov ah, [ebp+36] ; g2
		mov al, [ebp+40] ; b2

		movd mm1, eax ; mm1 contains color 1

		pxor mm7, mm7
		punpcklbw mm0, mm7
		punpcklbw mm1, mm7

		;; fill color

		;; prepare to fill the line buffer

		mov ebx, [ebp+12] ; index
		mov ecx, [ebp+16] ; width
		mov edi, [ebp+8]  ; dst

		;; calculate alpha

		xor edx, edx

		mov eax, ebx
		shl eax, 8 ; * 256
		div	ecx

		;; eax contains alpha

		movd mm2, eax
		punpcklwd mm2, mm2
		punpcklwd mm2, mm2

		mov edx, 0x00000100 ;; 256
		sub edx, eax

		movd mm3, edx
		punpcklwd mm3, mm3
		punpcklwd mm3, mm3

		;; do color blending

		movq mm4, mm0 ; color 1
		movq mm5, mm1 ; color 2

		pmullw mm4, mm2
		pmullw mm5, mm3

		paddw  mm4, mm5
		psrlw  mm4, 8

		;; pack and copy back

		packuswb  mm4, mm7
		movd [edi], mm4

		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi
		pop edi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;

cglobalfunc _desaturateFullSSE

;;
;;  void _desaturateFullSSE(unsigned int *dst,		[ebp+8]
;;							unsigned int  w,		[ebp+12]
;;							unsigned int  h)		[ebp+16]
;;

_desaturateFullSSE:

		;; function start

		push      ebp
        mov       ebp, esp

		;; parameters start at [ebp+8]

		push esi
		push eax
		push ebx
		push ecx
		push edx

		;; setup source = dest 
		;; and width*height counter

		mov  esi, [ebp+8]  ; dst
		mov  ebx, [ebp+16] ; h
		mov  ecx, [ebp+12] ; w
		mov  edx, [ebp+12] ; w copy

		pxor mm7, mm7

		;; prepare grey weighted multiplier

		mov   eax, 0x00000026 ;; alpha = 0 & 38/128 = r coeff
		movd  mm5, eax
		psllq mm5, 32		  ;; shift it to upper dword
		mov   eax, 0x004D000D ;; 77/128 = g coeff & 13/128 = b coeff
		movd  mm6, eax
		paddd mm5, mm6		  ;; mm5 has unpacked word coeffs

		mov   eax, 0xFFFF0000
		movd  mm6, eax		 
		psllq mm6, 32 ;; mm6 has mask for alpha

DDrunloopy:

		mov  ecx, edx ; w

DDrunloopx: 

		;; get source pixel

		movd mm0, [esi]
		punpcklbw mm0, mm7

		;; calculate grey 

		pmullw mm0, mm5 ; multiply by coeffs
		psrlw  mm0, 7   ; divide by 128
		psadbw mm0, mm7 ; add them into low word
		
		;; propagate grey to rgb, clear alpha

		pshufw mm1, mm0, 01000000b

		;; add alpha back

		pand   mm0, mm6
		paddw  mm1, mm0

		;; and copy back

		packuswb mm1, mm7
		movd [esi], mm1
		
		;; increment pointers

		add esi, 4

		dec ecx
		jnz DDrunloopx

		dec ebx
		jnz DDrunloopy


		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

;;
;;
;;

cglobalfunc _desaturateAmountSSE

;;
;;  void _desaturateAmountSSE(unsigned int *dst,		[ebp+8]
;;							  unsigned int  w,		[ebp+12]
;;							  unsigned int  h,		[ebp+16]
;;							  unsigned int amount);	[ebp+20]
;;

_desaturateAmountSSE:

		;; function start

		push      ebp
        mov       ebp, esp

		;; parameters start at [ebp+8]

		push esi
		push eax
		push ebx
		push ecx
		push edx

		;; setup source = dest 
		;; and width*height counter

		mov  esi, [ebp+8]  ; dst
		mov  ebx, [ebp+16] ; h
		mov  ecx, [ebp+12] ; w

		pxor mm7, mm7

		;; prepare grey weighted multiplier

		mov   eax, 0x00000026 ;; alpha = 0 & 38/128 = r coeff
		movd  mm5, eax
		psllq mm5, 32		  ;; shift it to upper dword
		mov   eax, 0x004D000D ;; 77/128 = g coeff & 13/128 = b coeff
		movd  mm6, eax
		paddd mm5, mm6		  ;; mm5 has unpacked word coeffs

		mov   eax, 0xFFFF0000
		movd  mm6, eax		 
		psllq mm6, 32 ;; mm6 has mask for alpha

		;; prepare multipliers

		mov  eax, [ebp+20] ; amount

		;; prepare unpacked mm2 & mm3
		;; mmx registers with alpha
		;; and (256 - alpha) values

		movd mm2, eax
		punpcklwd mm2, mm2
		punpcklwd mm2, mm2

		mov edx, 0x00000100 ;; 256
		sub edx, eax

		movd mm3, edx
		punpcklwd mm3, mm3
		punpcklwd mm3, mm3

		mov  edx, [ebp+12] ; w copy

DDArunloopy:

		mov  ecx, edx ; w

DDArunloopx: 

		;; get source pixel

		movd mm0, [esi]
		punpcklbw mm0, mm7
		movq mm4, mm0 ; make a copy

		;; calculate grey 

		pmullw mm0, mm5 ; multiply by coeffs
		psrlw  mm0, 7   ; divide by 128
		psadbw mm0, mm7 ; add them into low word
		
		;; propagate grey to rgb, clear alpha

		pshufw mm1, mm0, 01000000b

		;; add alpha back

		pand   mm0, mm6
		paddw  mm1, mm0

		;; alpha blend rgb & grey

		pmullw mm4, mm2
		pmullw mm1, mm3

		paddw  mm1, mm4
		psrlw  mm1, 8

		;; and copy back

		packuswb mm1, mm7
		movd [esi], mm1
		
		;; increment pointers

		add esi, 4

		dec ecx
		jnz DDArunloopx

		dec ebx
		jnz DDArunloopy


		;; all done

		pop edx
		pop ecx
		pop ebx
		pop eax
		pop esi

		;; clean up

		emms

		;; function ends
        
		mov       esp, ebp
        pop       ebp
        ret

