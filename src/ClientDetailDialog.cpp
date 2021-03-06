//
// This file is part of the aMule Project.
//
// Copyright (c) 2003-2008 aMule Team ( admin@amule.org / http://www.amule.org )
// Copyright (c) 2002-2008 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
// Any parts of this program derived from the xMule, lMule or eMule project,
// or contributed by third-party developers are copyrighted by their
// respective authors.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301, USA
//

#include "ClientDetailDialog.h"	// Interface declarations
#include "PartFile.h"		// Needed for CPartFile
#include "UploadQueue.h"	// Needed for CUploadQueue
#include "ServerList.h"		// Needed for CServerList
#include "amule.h"		// Needed for theApp
#include "Server.h"		// Needed for CServer
#include "updownclient.h"	// Needed for CUpDownClient
#include "muuli_wdr.h"		// Needed for ID_CLOSEWND
#include "Preferences.h"	// Needed for thePrefs

// CClientDetailDialog dialog

BEGIN_EVENT_TABLE(CClientDetailDialog,wxDialog)
	EVT_BUTTON(ID_CLOSEWND,CClientDetailDialog::OnBnClose)
END_EVENT_TABLE()


CClientDetailDialog::CClientDetailDialog(
	wxWindow *parent,
	CUpDownClient *client)
:
wxDialog(
	parent,
	9997,
	_("Client Details"),
	wxDefaultPosition,
	wxDefaultSize,
	wxDEFAULT_DIALOG_STYLE)
{
	m_client = client;
	wxSizer* content = clientDetails(this, true);
	OnInitDialog();
	content->SetSizeHints(this);
	content->Show(this, true);
}

CClientDetailDialog::~CClientDetailDialog()
{
}

void CClientDetailDialog::OnBnClose(wxCommandEvent& WXUNUSED(evt))
{
	EndModal(0);
}

bool CClientDetailDialog::OnInitDialog() {
	// Username, Userhash and Rating
	if (!m_client->GetUserName().IsEmpty()) {
		CastChild(ID_DNAME, wxStaticText)->SetLabel(
			m_client->GetUserName());
		// if we have client name we have userhash
		wxASSERT(!m_client->GetUserHash().IsEmpty());
		CastChild(ID_DHASH, wxStaticText)->SetLabel(
			m_client->GetUserHash().Encode());
		CastChild(ID_DRATING, wxStaticText)->SetLabel(
			wxString::Format(wxT("%u"), m_client->GetRating()));
	} else {
		CastChild(ID_DNAME, wxStaticText)->SetLabel(_("Unknown"));
		CastChild(ID_DHASH, wxStaticText)->SetLabel(_("Unknown"));
		CastChild(ID_DRATING, wxStaticText)->SetLabel(_("Unknown"));;
	}	
	
	// Client Software
	wxString OSInfo = m_client->GetClientOSInfo();
	if (!OSInfo.IsEmpty()) {
		CastChild(ID_DSOFT, wxStaticText)->SetLabel(
			m_client->GetSoftStr()+wxT(" (")+OSInfo+wxT(")"));
	} else {
		CastChild(ID_DSOFT, wxStaticText)->SetLabel(
			m_client->GetSoftStr());
	}

	// Client Version
	CastChild(ID_DVERSION, wxStaticText)->SetLabel(
		m_client->GetSoftVerStr());
	
	// User ID
	CastChild(ID_DID, wxStaticText)->SetLabel(
		CFormat(wxT("%u (%s)")) % m_client->GetUserIDHybrid() % (m_client->HasLowID() ? _("LowID") : _("HighID")));

	// Client IP/Port
	CastChild(ID_DIP, wxStaticText)->SetLabel(
		m_client->GetFullIP() +
		wxString::Format(wxT(":%i"), m_client->GetUserPort()));
	
	// Server IP/Port/Name
	if (m_client->GetServerIP()) {
		wxString srvaddr = Uint32toStringIP(m_client->GetServerIP());
		CastChild(ID_DSIP, wxStaticText)->SetLabel(
			srvaddr +
			wxString::Format(wxT(":%i"),m_client->GetServerPort()));
		CastChild(ID_DSNAME, wxStaticText)->SetLabel(
			m_client->GetServerName());
	} else {
		CastChild(ID_DSIP, wxStaticText)->SetLabel(_("Unknown"));
		CastChild(ID_DSNAME, wxStaticText)->SetLabel(_("Unknown"));
	}

	// Obfuscation
	wxString buffer;
	if (thePrefs::IsClientCryptLayerSupported()) {
		if (m_client->SupportsCryptLayer()) {
			if ((m_client->RequestsCryptLayer() || thePrefs::IsClientCryptLayerRequested()) && m_client->HasObfuscatedConnectionBeenEstablished()) {
				buffer = _("Enabled");
			} else {
				buffer = _("Supported");
			}
		} else {
			buffer = _("Not supported");
		}
	} else {
		buffer = _("Disabled");
	}
	CastChild(IDT_OBFUSCATION, wxStaticText)->SetLabel(buffer);

	// Kad
	if (m_client->GetKadPort()) {
		CastChild(IDT_KAD, wxStaticText)->SetLabel(_("Connected"));
	} else {
		CastChild(IDT_KAD, wxStaticText)->SetLabel(_("Disconnected"));
	}

	// File Name
	const CKnownFile* file = m_client->GetUploadFile();
	if (file) {
		wxString filename = MakeStringEscaped(
			TruncateFilename(file->GetFileName(), 60));
		CastChild(ID_DDOWNLOADING, wxStaticText)->SetLabel(filename);
	} else {
		CastChild(ID_DDOWNLOADING, wxStaticText)->SetLabel(wxT("-"));
	}
	
	// Upload
	CastChild(ID_DDUP, wxStaticText)->SetLabel(
		CastItoXBytes(m_client->GetTransferredDown()));
	
	// Download
	CastChild(ID_DDOWN, wxStaticText)->SetLabel(
		CastItoXBytes(m_client->GetTransferredUp()));
	
	// Average Upload Rate
	CastChild(ID_DAVUR, wxStaticText)->SetLabel(
		wxString::Format(_("%.1f kB/s"),
			m_client->GetKBpsDown()));
	
	// Average Download Rate
	CastChild(ID_DAVDR, wxStaticText)->SetLabel(
		wxString::Format(_("%.1f kB/s"),
			m_client->GetUploadDatarate() / 1024.0f));
	
	// Total Upload
	CastChild(ID_DUPTOTAL, wxStaticText)->SetLabel(
		CastItoXBytes(m_client->GetDownloadedTotal()));
	
	// Total Download
	CastChild(ID_DDOWNTOTAL, wxStaticText)->SetLabel(
		CastItoXBytes(m_client->GetUploadedTotal()));
	
	// DL/UP Modifier
	CastChild(ID_DRATIO, wxStaticText)->SetLabel(
		wxString::Format(wxT("%.1f"),
			(float)m_client->GetScoreRatio()));
	
	// Secure Ident
	if (theApp->CryptoAvailable()) {
		if (m_client->SUINotSupported()) {
			CastChild(IDC_CDIDENT, wxStaticText)->SetLabel(
				_("Not supported"));
		} else if (m_client->SUIFailed()) {
			CastChild(IDC_CDIDENT, wxStaticText)->SetLabel(
				_("Failed"));
		} else if (m_client->SUINeeded()) {
			CastChild(IDC_CDIDENT, wxStaticText)->SetLabel(
				_("Not complete"));
		} else if (m_client->IsBadGuy()) {
			CastChild(IDC_CDIDENT, wxStaticText)->SetLabel(
				_("Bad Guy"));
		} else if (m_client->IsIdentified()) {
			CastChild(IDC_CDIDENT, wxStaticText)->SetLabel(
				_("Verified - OK"));
		}	
	} else {
		CastChild(IDC_CDIDENT, wxStaticText)->SetLabel(
			_("Not Available"));
	}
	
	// Queue Score
	if (m_client->GetUploadState() != US_NONE) {
		CastChild(ID_DSCORE, wxStaticText)->SetLabel(
			wxString::Format(_("%u (QR: %u)"),
				m_client->GetScore(
					false, m_client->IsDownloading(), false),
			theApp->uploadqueue->GetWaitingPosition(m_client)));		
	} else {
		CastChild(ID_DSCORE, wxStaticText)->SetLabel(wxT("-"));
	}
	Layout();
	
	return true;
}
// File_checked_for_headers
