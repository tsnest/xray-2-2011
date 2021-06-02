////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.02.2008
//	Author		: Konstantin Slipchenko
//	Description : contact info buffer
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_CONTACT_INFO_BUFFER_H_INCLUDED
#define XRAY_CONTACT_INFO_BUFFER_H_INCLUDED

class body;
class island;

class contact_info_buffer
{
private:
	vector<contact_info> m_contacts;

public:
	const physics::vector<contact_info>& contacts( )const	{ return m_contacts; }

public:
	void	add( const contact_info	&i )	{ m_contacts.push_back( i ); }

};

class body;
class island;

bool create_contact_joints( island& isl, const contact_info_buffer &contacts, body& body_0, body& body_1 );
bool create_contact_joints( island& isl, const contact_info_buffer &contacts, body& body );

#endif