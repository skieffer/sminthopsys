/*
 * Garuda plugin - Enables communication with other Garuda enabled apps.
 *
 * Copyright (C) 2011-2012  Monash University
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 *
 *
 * Author: Michael Wybrow <mjwybrow@users.sourceforge.net>
*/

#include <cassert>

#include <QMessageBox>

#include "garudaclient.h"
#include "qt-json/json.h"


/**
 * parent: a QObject, such as a QApplication object, which is to "own" this
 *   client object. May be set to 0 if there is no obvious parent.
 *
 * gadgetName: the name of the gadget
 *
 * gadgetUUID: the UUID of the gadget
 *
 * providerName: the name and affiliation of the provider of the gadget
 *
 * description: a description of the gadget
 *
 * categoryList: a list of Garuda-approved category names, to which this
 *   gadget belongs
 *
 * inputFiles: an even-length list of strings, giving extension-format pairs
 *   for files that this gadget can open.
 *   For example, [ "xml", "sbml" ]. If you provide an odd number of strings,
 *   the final entry will simply be ignored.
 *
 * outputFiles: same format as inputFiles, this time specifying the kinds of
 *   files that this gadget creates.
 *
 * iconPath: absolute path to icon file
 *
 * screenshotPaths: list of absolute paths to screenshot files
 *
 * launchCommand: the full command line string that launches the gadget
 */
GarudaClient::GarudaClient(QObject *parent, QString gadgetName,
                           QString gadgetUUID, QString providerName,
                           QString description, QList<QString> categoryList,
                           QList<QString> inputFiles, QList<QString> outputFiles,
                           QString iconPath, QList<QString> screenshotPaths,
                           QString launchCommand) :
    QObject(parent),
    m_tcp_socket(NULL),
    m_gadget_name(gadgetName),
    m_gadget_uuid(gadgetUUID),
    m_provider_name(providerName),
    m_description(description),
    m_categoryList(categoryList),
    m_inputFiles(inputFiles),
    m_outputFiles(outputFiles),
    m_iconPath(iconPath),
    m_screenshotPaths(screenshotPaths),
    m_launchCommand(launchCommand)
{
    openSocket();

    activateGadgetWithCore();
    registerGadget();
}

void GarudaClient::openSocket(void)
{
    if (m_tcp_socket == NULL)
    {
        m_tcp_socket = new QTcpSocket();

        connect(m_tcp_socket, SIGNAL(readyRead()), this, SLOT(readData()));

        connect(m_tcp_socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)));

        m_tcp_socket->connectToHost("localhost", 9000);

        const int timeout = 5 * 1000; // 5 seconds.
        if (!m_tcp_socket->waitForConnected(timeout))
        {
            delete m_tcp_socket;
            m_tcp_socket = NULL;
            return;
        }
    }
}

void GarudaClient::closeSocket(void)
{
    m_tcp_socket->disconnectFromHost();
    m_tcp_socket->waitForDisconnected();
    delete m_tcp_socket;
    m_tcp_socket = NULL;
}

bool GarudaClient::connectedToServer(void) const
{
    return (m_tcp_socket &&
            (m_tcp_socket->state() == QAbstractSocket::ConnectedState));
}

void GarudaClient::sendData(const QString& string)
{
    if (!connectedToServer())
    {
        return;
    }

    qDebug() << "GarudaClient Send: " << string;

    QByteArray block = string.toUtf8().data();
    block.append("\n");
    int written = m_tcp_socket->write(block);
    Q_UNUSED (written)
    assert (written == block.size());
    m_tcp_socket->flush();
}

void GarudaClient::readData(void)
{
    assert(m_tcp_socket);
    QByteArray in;

    while (m_tcp_socket->bytesAvailable() > 0)
    {
        in += m_tcp_socket->read(1000);
        //qDebug() << "Read Data: " << in;
    }

    int sentinelPos = -1;
    while ((sentinelPos = in.indexOf('\n')) != -1)
    {
        QByteArray message = in.left(sentinelPos);
        in.remove(0, sentinelPos + 1);
        qDebug() << "GarudaClient Receive: " << message;
        parseMessage(message);
        emit newMessage(message);
    }
}

void GarudaClient::parseMessage(const QString &message)
{
    QVariant var = QtJson::Json::parse(message);
    QVariantMap data = var.toMap();

    QVariantMap header = data["header"].toMap();
    QString action =  header["id"].toString();

    if (action == "GetCompatibleGadgetListResponse")
    {
        emit showCompatibleSoftwareResponse(data);
    }
    else if (action == "LoadDataRequest")
    {
        QVariantMap body = data["body"].toMap();
        QString filePath = body["data"].toString();
        QString originGadgetName = body["originGadgetName"].toString();
        QString originGadgetUUID = body["originGadgetUUID"].toString();
        qDebug("Garuda: %s (%s) requests we load %s",
               qPrintable(originGadgetName), qPrintable(originGadgetUUID),
               qPrintable(filePath));
        emit fileOpenRequest(filePath);
    }
}

void GarudaClient::activateGadgetWithCore(void)
{
    QVariantMap header;
    header["id"] = "ActivateGadgetRequest";
    header["version"] = "0.1";

    QVariantMap body;
    body["gadgetName"] = m_gadget_name;
    body["gadgetUUID"] = m_gadget_uuid;

    QVariantMap root;
    root["header"] = header;
    root["body"] = body;

    QByteArray data = QtJson::Json::serialize(root);

    sendData(QString(data));
}

void GarudaClient::registerGadget(void)
{
    QVariantMap header;
    header["id"] = "RegisterGadgetRequest";
    header["version"] = "0.1";

    QVariantList categoryList;
    foreach (QString category, m_categoryList)
    {
        categoryList.append(QVariant(category));
    }

    QVariantList screenshots;
    foreach (QString screenshotPath, m_screenshotPaths)
    {
        screenshots.append(screenshotPath);
    }

    QVariantList inputFileFormats;
    // If length of list m_inputFiles is odd, last entry is simply ignored.
    int numInputPairs = (m_inputFiles.length() - m_inputFiles.length()%2)/2;
    for (int i = 0; i < numInputPairs; i++)
    {
        QVariantMap format;
        format["fileExtension"] = m_inputFiles.at(2*i);
        format["fileFormat"] = m_inputFiles.at(2*i+1);
        inputFileFormats.append(format);
    }

    QVariantList outputFileFormats;
    // If length of list m_outputFiles is odd, last entry is simply ignored.
    int numOutputPairs = (m_outputFiles.length() - m_outputFiles.length()%2)/2;
    for (int i = 0; i < numOutputPairs; i++)
    {
        QVariantMap format;
        format["fileExtension"] = m_outputFiles.at(2*i);
        format["fileFormat"] = m_outputFiles.at(2*i+1);
        outputFileFormats.append(format);
    }

    QVariantMap body;
    body["categoryList"] = categoryList;
    body["description"] = m_description;
    body["gadgetUUID"] = m_gadget_uuid;
    body["iconPath"] = m_iconPath;
    body["inputFileFormats"] = inputFileFormats;
    body["outputFileFormats"] = outputFileFormats;
    body["launchCommand"] = m_launchCommand;
    body["name"] = m_gadget_name;
    body["provider"] = m_provider_name;
    body["screenshots"] = screenshots;

    QVariantMap root;
    root["header"] = header;
    root["body"] = body;

    QByteArray data = QtJson::Json::serialize(root);

    sendData(QString(data));
}

void GarudaClient::loadFileIntoSoftware(QFileInfo fileInfo, QString softwareName,
        QString softwareUUID)
{
    QVariantMap header;
    header["id"] = "SentDataToGadgetRequest";
    header["version"] = "0.1";

    QVariantMap body;
    body["data"] = fileInfo.absoluteFilePath();
    body["gadgetName"] = m_gadget_name;
    body["gadgetUUID"] = m_gadget_uuid;
    body["targetGadgetName"] = softwareName;
    body["targetGadgetUUID"] = softwareUUID;

    QVariantMap root;
    root["header"] = header;
    root["body"] = body;

    QByteArray data = QtJson::Json::serialize(root);

    sendData(QString(data));
}

void GarudaClient::showCompatibleSoftwareFor(QString extension, QString format)
{
    QVariantMap header;
    header["id"] = "GetCompatibleGadgetListRequest";
    header["version"] = "0.1";

    QVariantMap body;
    body["gadgetName"] = m_gadget_name;
    body["gadgetUUID"] = m_gadget_uuid;
    body["fileExtension"] = extension;
    body["fileType"] = format;

    QVariantMap root;
    root["header"] = header;
    root["body"] = body;

    QByteArray data = QtJson::Json::serialize(root);

    sendData(QString(data));
}


void GarudaClient::displayError(QAbstractSocket::SocketError socketError)
{
    QWidget *widget = NULL;
    switch (socketError)
    {
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(widget, tr("Garuda Gadget: Network Error"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
            break;
        case QAbstractSocket::SocketTimeoutError:
        case QAbstractSocket::ConnectionRefusedError:
            return;
            QMessageBox::information(widget, tr("Garuda Gadget: Network Error"),
                                 tr("Cannot connect to the Garuda core "
                                    "server.  Please check your network "
                                    "connection and try again later."));
            break;
        default:
            QMessageBox::information(widget, tr("Garuda Gadget: Network Error"),
                                     tr("The following error occurred: %1.")
                                     .arg(m_tcp_socket->errorString()));
    }
}

// vim: filetype=cpp ts=4 sw=4 et tw=0 wm=0 cindent
