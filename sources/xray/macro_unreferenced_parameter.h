#ifndef XRAY_UNREFERENCED_PARAMETER_H_INCLUDED
#define XRAY_UNREFERENCED_PARAMETER_H_INCLUDED


namespace xray {
namespace detail {
	inline void	unreferenced_parameter_helper ( ... ) {}
} // namespace detail
} // namespace xray

#define XRAY_UNREFERENCED_PARAMETER(parameter) \
			(void)(&parameter)

#define XRAY_UNREFERENCED_PARAMETERS(...) \
	if ( xray::identity(false) ) { xray::detail::unreferenced_parameter_helper(__VA_ARGS__); } else (void)0

#endif // #ifndef XRAY_UNREFERENCED_PARAMETER_H_INCLUDED