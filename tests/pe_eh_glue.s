# The pieces of vcruntime140 an MSVC C++ object links against, for pe_eh.cpp:
# the import thunks of __CxxFrameHandler3 and _CxxThrowException (the
# fixture imports both from pehost.dll) and type_info's vftable symbol, which
# the type descriptors point to (never called).
	.text
	.globl	__CxxFrameHandler3
__CxxFrameHandler3:
	jmpq	*__imp___CxxFrameHandler3(%rip)
	.globl	_CxxThrowException
_CxxThrowException:
	jmpq	*__imp__CxxThrowException(%rip)
	.data
	.globl	"??_7type_info@@6B@"
"??_7type_info@@6B@":
	.quad	0
	.text
# operator delete(void *, size_t), for the deleting destructors (never called).
	.globl	"??3@YAXPEAX_K@Z"
"??3@YAXPEAX_K@Z":
	retq
