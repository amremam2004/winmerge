/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997  Dean P. Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  MergeDoc.h
 *
 * @brief Declaration of CMergeDoc class
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#if !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
#define AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "GhostTextBuffer.h"
#include <vector>
#include "afxtempl.h"
#include "DiffWrapper.h"


/**
 * @brief additionnal action code for WinMerge (reserve 100 first codes for CrystalEdit)
 */enum
{
	CE_ACTION_MERGE = 100,
};

/**
 * @brief Return statuses of file rescan
 */
enum
{
	RESCAN_OK = 0,
	RESCAN_SUPPRESSED,
	RESCAN_IDENTICAL,
	RESCAN_BINARIES,
	RESCAN_FILE_ERR
};

class CMergeEditView;
class CMergeDiffDetailView;

//<jtuc 2003-06-28>
/*
class CUndoItem
{
public:
	UINT begin,end,diffidx;
	int blank;
	BOOL bInsert;
	CMergeEditView *m_pList;
	CStringList list;
};
*/
//<jtuc>

class CChildFrame;
class CDirDoc;

/**
 * @brief Document class for merging two files
 */
class CMergeDoc : public CDocument
{
// Attributes
public:
class CDiffTextBuffer : public CGhostTextBuffer
	{
		friend class CMergeDoc;
private :
		CMergeDoc * m_pOwnerDoc;
		BOOL m_bIsLeft;
		BOOL FlagIsSet(UINT line, DWORD flag);
		CString m_strTempPath;

		int DetermineCRLFStyle(LPVOID lpBuf, DWORD dwLength);
		void ReadLineFromBuffer(TCHAR *lpLineBegin, DWORD dwLineLen = 0);
public :
		void SetTempPath(CString path);
		virtual void AddUndoRecord (BOOL bInsert, const CPoint & ptStartPos, const CPoint & ptEndPos,
                        LPCTSTR pszText, int flags, int nActionType = CE_ACTION_UNKNOWN);
		bool curUndoGroup();
		void ReplaceLine(CCrystalTextView * pSource, int nLine, const CString& strText, int nAction =CE_ACTION_UNKNOWN);
		void ReplaceFullLine(CCrystalTextView * pSource, int nLine, const CString& strText, int nAction =CE_ACTION_UNKNOWN);

		BOOL LoadFromFile(LPCTSTR pszFileName, int nCrlfStyle = CRLF_STYLE_AUTOMATIC);
		BOOL SaveToFile (LPCTSTR pszFileName, BOOL bTempFile,
				int nCrlfStyle = CRLF_STYLE_AUTOMATIC, 
											 BOOL bClearModifiedFlag = TRUE );

		CDiffTextBuffer (CMergeDoc * pDoc, BOOL bLeft)
		{
			m_pOwnerDoc = pDoc;
			m_bIsLeft=bLeft;
		}
		// If line has text (excluding eol), set strLine to text (excluding eol)
		BOOL GetLine( int nLineIndex, CString &strLine ) 
		{ 
			int nLineLength = CCrystalTextBuffer::GetLineLength 
				( nLineIndex ); 
			
			if( nLineLength < 0 ) 
				return FALSE; 
			else if( nLineLength == 0 ) 
				strLine.Empty(); 
			else 
			{ 
				_tcsncpy ( strLine.GetBuffer( nLineLength + 1 ), 
					CCrystalTextBuffer::GetLineChars( nLineIndex ), 
					nLineLength ); 
				strLine.ReleaseBuffer( nLineLength ); 
			} 
			return TRUE; 
		} 
		// if line has any text (including eol), set strLine to text (including eol)
		BOOL GetFullLine(int nLineIndex, CString &strLine)
		{
			if (!GetFullLineLength(nLineIndex))
				return FALSE;
			strLine = GetLineChars(nLineIndex);
			return TRUE;
		}

		virtual void SetModified (BOOL bModified = TRUE)
		{
			CCrystalTextBuffer::SetModified (bModified);
			m_pOwnerDoc->SetModifiedFlag (bModified);
		}
		void InsertLine (LPCTSTR pszLine, int nLength = -1, int nPosition = -1)
		{
			CCrystalTextBuffer::InsertLine(pszLine, nLength, nPosition);
		}

		void prepareForRescan();

		/** 
		After editing a line, we don't know if there is a diff or not.
		So we clear the LF_DIFF flag (and it is more easy to read during edition).
		Rescan will set the proper color
		*/
		virtual void OnNotifyLineHasBeenEdited(int nLine);


	} friend;

// End declaration of CMergeDoc::CDiffTextBuffer

// Begin declaration of CMergeDoc

	CDiffTextBuffer m_ltBuf;
	CDiffTextBuffer m_rtBuf;

protected: // create from serialization only
	CMergeDoc();
	DECLARE_DYNCREATE(CMergeDoc)

	
	// Operations
public:	
	CPtrList m_undoList;
	CArray<DIFFRANGE,DIFFRANGE> m_diffs;
	UINT m_nDiffs;
	CString m_strLeftFile, m_strRightFile;

	void ReadSettings();
	BOOL OpenDocs(CString sLeftFile, CString sRightFile,
		BOOL bROLeft = FALSE, BOOL bRORight = FALSE);
	int LoadFile(CString sFileName, BOOL bLeft);
	void RescanIfNeeded(float timeOutInSecond);
	int Rescan(BOOL bForced = FALSE);
	void ShowRescanError(int nRescanResult);
	void AddUndoAction(UINT nBegin, UINT nEnd, UINT nDiff, int nBlanks, BOOL bInsert, CMergeEditView *pList);
	BOOL Undo();
	void CopyAllList(bool bSrcLeft);
	void ListCopy(bool bSrcLeft);
	BOOL TrySaveAs(CString strPath, BOOL &bSaveSuccess, BOOL bLeft);
	BOOL DoSave(LPCTSTR szPath, BOOL &bSaveSuccess, BOOL bLeft);
	//CString ExpandTabs(LPCTSTR szText);
	//CString Tabify(LPCTSTR szText);
	int LineToDiff(UINT nLine);
	BOOL LineInDiff(UINT nLine, UINT nDiff);
	void SetDiffViewMode(BOOL bEnable);
	void Showlinediff(CMergeEditView * pView);
	void Showlinediff(CMergeDiffDetailView * pView);
	RECT Computelinediff(CCrystalTextView * pView, CCrystalTextView * pOther);
	
	void SetMergeViews(CMergeEditView * pLeft, CMergeEditView * pRight);
	void SetMergeDetailViews(CMergeDiffDetailView * pLeft, CMergeDiffDetailView * pRight);
	void SetDirDoc(CDirDoc * pDirDoc);
	void DirDocClosing(CDirDoc * pDirDoc);
	BOOL CloseNow();

	CMergeEditView * GetLeftView() { return m_pLeftView; }
	CMergeEditView * GetRightView() { return m_pRightView; }
	CMergeDiffDetailView * GetLeftDetailView() { return m_pLeftDetailView; }
	CMergeDiffDetailView * GetRightDetailView() { return m_pRightDetailView; }
	CChildFrame * GetParentFrame();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMergeDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual BOOL SaveModified();
	virtual void DeleteContents ();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL SaveHelper();
	std::vector<CMergeEditView*> undoTgt;
	std::vector<CMergeEditView*>::iterator curUndo;
	void FlushAndRescan(BOOL bForced = FALSE);
	BOOL TempFilesExist();
	void CleanupTempFiles();
	BOOL InitTempFiles(const CString& srcPathL, const CString& strPathR);
	void SetCurrentDiff(int nDiff);
	int GetCurrentDiff() { return m_nCurDiff; }
	UINT CountPrevBlanks(UINT nCurLine, BOOL bLeft);
	virtual ~CMergeDoc();
	virtual void OnFileEvent (WPARAM wEvent, LPCTSTR pszPathName);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation data
protected:
	int m_nCurDiff;		// 0-based index, -1 if no diff selected
	CString m_strTempRightFile;
	CString m_strTempLeftFile;
	CMergeEditView * m_pLeftView;
	CMergeEditView * m_pRightView;
	CMergeDiffDetailView * m_pLeftDetailView;
	CMergeDiffDetailView * m_pRightDetailView;
	CDirDoc * m_pDirDoc;
	BOOL m_bEnableRescan;
	COleDateTime m_LastRescan;
	CDiffWrapper m_diffWrapper;

// friend access
	friend class RescanSuppress;


// Generated message map functions
protected:
	//{{AFX_MSG(CMergeDoc)
	afx_msg void OnFileSave();
	afx_msg void OnUpdateStatusNum(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PrimeTextBuffers();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MERGEDOC_H__BBCD4F90_34E4_11D1_BAA6_00A024706EDC__INCLUDED_)
