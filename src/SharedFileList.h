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

#ifndef SHAREDFILELIST_H
#define SHAREDFILELIST_H

#include <map>
#include <wx/thread.h>		// Needed for wxMutex

#include "Types.h"		// Needed for uint16 and uint64

struct UnknownFile_Struct;

class CKnownFileList;
class CKnownFile;
class CMemFile;
class CMD4Hash;
class CServer;
class CUpDownClient;
class CPublishKeywordList;
class CPath;
class CAICHHash;


typedef std::map<CMD4Hash,CKnownFile*> CKnownFileMap;

class CSharedFileList {
public:
	CSharedFileList(CKnownFileList* in_filelist);
	~CSharedFileList();
	void 	Reload();
	void	SafeAddKFile(CKnownFile* toadd, bool bOnlyAdd = false);
	void	RemoveFile(CKnownFile* toremove);
	CKnownFile*	GetFileByID(const CMD4Hash& filehash);
	short	GetFilePriorityByID(const CMD4Hash& filehash);
	const CKnownFile* GetFileByIndex(unsigned int index) const;
	void	CreateOfferedFilePacket(CKnownFile* cur_file, CMemFile* files, CServer* pServer, CUpDownClient* pClient);
	size_t	GetCount()	{ wxMutexLocker lock(list_mut); return m_Files_map.size(); }
	size_t  GetFileCount()	{ wxMutexLocker lock(list_mut); return m_Files_map.size(); }
	void	CopyFileList(std::vector<CKnownFile*>& out_list);
	void	UpdateItem(CKnownFile* toupdate);
	unsigned	AddFilesFromDirectory(const CPath& directory);
	void    GetSharedFilesByDirectory(const wxString& directory, CKnownFilePtrList& list);
	void	ClearED2KPublishInfo();
	void	RepublishFile(CKnownFile* pFile);
	void	Process();
	void	PublishNextTurn()	{ m_lastPublishED2KFlag = true; }
	bool	RenameFile(CKnownFile* pFile, const CPath& newName);
	
	/* Kad Stuff */
	void	Publish();
	void	AddKeywords(CKnownFile* pFile);
	void	RemoveKeywords(CKnownFile* pFile);	
	// This is actually unused, but keep it here - will be needed later.
	void	ClearKadSourcePublishInfo();

	/** 
 	 * Checks for files which missing or wrong AICH hashes.
 	 * Those that are found are scheduled for ACIH hashing.
 	 */
	void CheckAICHHashes(const std::list<CAICHHash>& hashes);
	
private:
	bool	AddFile(CKnownFile* pFile);
	void	FindSharedFiles();
	bool	reloading;
	
	void	SendListToServer();
	uint32 m_lastPublishED2K;
	bool	 m_lastPublishED2KFlag;	

	CKnownFileList*	filelist;

	CKnownFileMap		m_Files_map;
	mutable wxMutex		list_mut;

	
	/* Kad Stuff */
	CPublishKeywordList* m_keywords;
	unsigned int m_currFileSrc;
	unsigned int m_currFileNotes;
	unsigned int m_currFileKey;
	uint32 m_lastPublishKadSrc;
	uint32 m_lastPublishKadNotes;
};

#endif // SHAREDFILELIST_H
// File_checked_for_headers
