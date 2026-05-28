#pragma once

// データの種類を定数で定義
#define DSTYPE_FILE 0
#define DSTYPE_WEB  1

class CDataSource : public CObject
{
public:
    CDataSource();
    virtual ~CDataSource();

    // --- プロパティ (MFC CStringで柔軟に管理) ---
    CString m_strPath;        // URL または ローカルパス
    int     m_nType;          // DSTYPE_FILE or DSTYPE_WEB
    CString m_strId;          // Basic認証用ID
    CString m_strPass;        // Basic認証用パスワード
    CString m_strDescription; // 説明文
    CString m_strName;        // 名称

    // --- インターフェース ---
    // 派生クラスで、ファイル読み込みやWinInet通信を実装
    virtual BOOL Load() = 0;

    // 必要に応じてシリアライズ等も対応可能
    // virtual void Serialize(CArchive& ar);
};
