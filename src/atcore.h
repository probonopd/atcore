#pragma once

#include <QObject>
#include <QList>
#include <QSerialPortInfo>

class SerialLayer;
class IFirmware;

enum PrinterState {
    DISCONNECTED, //Not Connected to a printer, initial state
    CONNECTING, //Attempting to connect, Fw not probed
    IDLE, //Connected to printer and ready for commands
    BUSY, //Printer is working
    PAUSE, //Printer is paused
    ERROR, // Printer Returned Error
    STOP, // Emergency Stop
    STARTPRINT, //Just Starting a print job
    FINISHEDPRINT, //Just Finished print job
};

struct AtCorePrivate;
enum AXIS {
    X = 1 << 0,
    Y = 1 << 1,
    Z = 1 << 2,
    E = 1 << 3,
};

class AtCore : public QObject
{
    Q_OBJECT
public:
    AtCore(QObject *parent = nullptr);
    QList<QSerialPortInfo> serialPorts() const;

    void initFirmware(const QString &port, int baud);
    bool isInitialized();
    void setSerial(SerialLayer *serial);
    SerialLayer *serial() const;
    void setPlugin(IFirmware *plugin);
    IFirmware *plugin() const;

    /**
     * @brief Get Printer state
     * @return State of the printer
     */
    PrinterState state(void);

    /**
     * @brief setState: set Printer state
     * @param state : printer state.
     */
    void setState(PrinterState state);

    /**
     * @brief Return true if plugin is loaded
     */
    bool pluginLoaded();

    /**
     * @brief Request firmware sending M115 command
     */
    void requestFirmware();

    /**
     * @brief Return printed percentage
     */
    float percentagePrinted();

    /**
     * @brief Request a list of firmware plugins
     */
    QStringList availablePlugins();

    /**
     * @brief Load A firmware
     *
     * @param fwName : name of the firmware
     */
    void loadFirmware(const QString &fwName);

    /**
     * @brief detectFirmware attempt to autodetect the firmware
     */
    void detectFirmware();

    /**
     * @brief Return FIFO command from printer
     *
     * @return QByteArray
     */
    QByteArray popCommand();

    /**
     * @brief Translate command and push it
     *
     * @param msg : Command
     */
    void pushCommand(const QString &comm);

signals:
    /**
     * @brief Emit signal when the printing precentabe changes.
     *
     * @param msg : Message
     */
    void printProgressChanged(const float &newProgress);

    /**
     * @brief Emit signal when message is received from the printer
     *
     * @param msg : Message
     */
    void receivedMessage(const QByteArray &message);

    /**
     * @brief State Changed
     * @param newState : the new state of the printer
     */
    void stateChanged(PrinterState newState);

public slots:
    /**
     * @brief Public Interface for printing a file
     */
    void print(const QString &fileName);

    /**
     * @brief Stop the Printer
     */
    void stop();

    /**
     * @brief Send home command
     * @param axis: the axis(es) to home (use X Y Z or any combo of)
     */
    void home(uchar axis);

    /**
     * @brief Send home all command
     */
    void home();

    /**
     * @brief Set extruder temperature
     * @param temp : new temperature
     * @param extruder : extruder number
     */
    void setExtruderTemp(uint temp = 0, uint extruder = 0);

    /**
     * @brief Set the bed temperature
     * @param temp : new temperature
     */
    void setBedTemp(uint temp = 0);

    /**
     * @brief setFanSpeed set the fan speed
     * @param fanNumber: fan number
     * @param speed: new speed of the fan 0-100
     */
    void setFanSpeed(uint speed = 0, uint fanNumber = 0);

private:
    /**
     * @brief Print a file
     *
     * @param fileName : path of file to print
     */
    void printFile(const QString &fileName);

    void newMessage(const QByteArray &message);
    void findFirmware(const QByteArray &message);
    void findPlugins();
    QByteArray lastMessage;
    AtCorePrivate *d;
    PrinterState printerState;
    float percentage;
};
