#ifndef REVIVEMANIFESTCONTROLLER_H
#define REVIVEMANIFESTCONTROLLER_H

#include <QObject>
#include <QDir>
#include <QJsonDocument>

class CReviveManifestController : public QObject
{
	Q_OBJECT
	typedef QObject BaseClass;

public:
	static CReviveManifestController *SharedInstance();

	const char* AppKey = "revive.dashboard.overlay";
	const char* AppDir = "Revive/";

public:
	CReviveManifestController();
	virtual ~CReviveManifestController();

	Q_INVOKABLE bool addManifest(const QString &canonicalName, const QString &manifest);
	Q_INVOKABLE bool loadManifest(const QString &canonicalName);
	Q_INVOKABLE bool removeManifest(const QString &canonicalName);
	Q_INVOKABLE bool launchApplication(const QString &canonicalName);
	Q_INVOKABLE bool isApplicationInstalled(const QString &canonicalName);

private:
	QDir m_manifestDir;

	bool SaveManifest(QFile &file, const QJsonDocument& document);
};

#endif // REVIVEMANIFESTCONTROLLER_H
