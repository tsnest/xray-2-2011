////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DOCUMENT_EVENT_ARGS_H_INCLUDED
#define DOCUMENT_EVENT_ARGS_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		namespace controls 
		{

			public ref class document_event_args : EventArgs
			{
			public:
				document_event_args(document_base^ set_document):document(set_document)
				{}

				document_base^ document;

			}; // class document_event_args

		} // namespace controls
	} // namespace editor
} // namespace xray

#endif // #ifndef DOCUMENT_EVENT_ARGS_H_INCLUDED