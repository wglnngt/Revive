#include "revivemanifestcontroller.h"
#include "openvr.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

CReviveManifestController *s_pSharedRevController = NULL;

CReviveManifestController *CReviveManifestController::SharedInstance()
{
	if ( !s_pSharedRevController )
	{
		s_pSharedRevController = new CReviveManifestController();
	}
	return s_pSharedRevController;
}

CReviveManifestController::CReviveManifestController()
	: BaseClass()
	, m_manifestDir("Revive")
{
	// Check if the main manifest is present, if not create it.
	if (!vr::VRApplications()->IsApplicationInstalled(AppKey))
	{
		// Initialize the Json structure
		QJsonObject strings, english;
		english["name"] = "Revive Dashboard";
		english["description"] = "Revive Dashboard overlay";
		strings["en_us"] = english;

		QJsonObject overlay;
		overlay["app_key"] = AppKey;
		overlay["launch_type"] = "binary";
		overlay["binary_path_windows"] = "ReviveOverlay.exe";
		overlay["arguments"] = "";
		overlay["image_path"] = "revive.png";
		overlay["is_dashboard_overlay"] = true;
		overlay["strings"] = strings;

		QJsonObject manifest;
		QJsonArray applications;
		applications.append(overlay);
		manifest["applications"] = applications;

		// Write the Json structure to the manifest file
		QFile manifestFile("revive.vrmanifest");
		QJsonDocument doc(manifest);
		SaveManifest(manifestFile, doc);

		QFileInfo info(manifestFile);
		QString nativePath = QDir::toNativeSeparators(info.absoluteFilePath());
		vr::VRApplications()->AddApplicationManifest(nativePath.toUtf8());
	}

	// TODO: Auto-launch the Revive dashboard.
	//vr::VRApplications()->SetApplicationAutoLaunch(AppKey, true);
}

CReviveManifestController::~CReviveManifestController()
{
}

bool CReviveManifestController::SaveManifest(QFile &file, const QJsonDocument& document)
{
	if (!file.open(QIODevice::WriteOnly))
	{
		qWarning("Couldn't open manifest file for writing.");
		return false;
	}

	QByteArray array = document.toJson();
	file.write(array);
	file.close();
	return true;
}

bool CReviveManifestController::addManifest(const QString &canonicalName, const QString &manifest)
{
	// Validate the manifest
	QJsonParseError parse;
	QJsonDocument doc = QJsonDocument::fromJson(manifest.toUtf8(), &parse);
	if (parse.error != QJsonParseError::NoError)
	{
		qDebug(parse.errorString().toUtf8());
		return false;
	}

	// Save the manifest
	QString filePath = m_manifestDir.absoluteFilePath(canonicalName + ".vrmanifest");
	QFile file(filePath);
	if (!SaveManifest(file, doc))
		return false;

	QString nativePath = QDir::toNativeSeparators(filePath);
	vr::EVRApplicationError error = vr::VRApplications()->AddApplicationManifest(nativePath.toUtf8(), true);
	return error == vr::VRApplicationError_None;
}

bool CReviveManifestController::loadManifest(const QString &canonicalName)
{
	// Check if manifest exists
	QString filePath = m_manifestDir.absoluteFilePath(canonicalName + ".vrmanifest");
	QFile file(filePath);
	if (!file.exists())
		return false;

	// Add the manifest as a temporary application
	QString nativePath = QDir::toNativeSeparators(filePath);
	vr::EVRApplicationError error = vr::VRApplications()->AddApplicationManifest(nativePath.toUtf8(), true);
	if (error != vr::VRApplicationError_None)
		return false;

	// Check if the manifest is actually loaded
	if (vr::VRApplications()->IsApplicationInstalled(("revive.app." + canonicalName).toUtf8()))
		return true;
	else
		return false;
}

bool CReviveManifestController::removeManifest(const QString &canonicalName)
{
	return m_manifestDir.remove(canonicalName + ".vrmanifest");
}

bool CReviveManifestController::launchApplication(const QString &appKey)
{
	return vr::VRApplications()->LaunchApplication(appKey.toUtf8()) == vr::VRApplicationError_None;
}

bool CReviveManifestController::isApplicationInstalled(const QString &appKey)
{
	return vr::VRApplications()->IsApplicationInstalled(appKey.toUtf8());
}
