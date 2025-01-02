#ifndef QTLOGSINK_H
#define QTLOGSINK_H

#include <spdlog/sinks/base_sink.h>
#include <QTextEdit>
#include <QMetaObject>
#include <mutex>
#include <QTimer>
#include <QThread>

class QtLogSink : public QObject, public spdlog::sinks::base_sink<std::mutex> {
    Q_OBJECT
   
public:
    explicit QtLogSink(QTextEdit* textEdit, QObject* parent = nullptr)
        : QObject(parent), textEdit_(textEdit) {}
protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {

        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
        QString qstr = QString::fromStdString(fmt::to_string(formatted));

        QString color;
        qstr.replace("\n", "<br>"); //替换一下换行符 不然不换行
        switch (msg.level)
        {
        case spdlog::level::info:
            qstr.replace("[info]", "<font color=\"green\">[info]</font>");
            break;
        case spdlog::level::warn:
            qstr.replace("[warning]", "<font color=\"orange\">[warning]</font>");
            break;
        case spdlog::level::err:
            qstr.replace("[error]", "<font color=\"red\">[error]</font>");
            break;
        default:
            break;
        }

     
        try //退出的时候会因为主线程退出的原因，这里报错。先这样凑和处理一下
        {
            if (textEdit_ && QThread::currentThread() == textEdit_->thread()) {
               
            }
            std::string stdStr = qstr.toStdString();
       /*    const char* charPtr = stdStr.c_str();
            QMetaObject::invokeMethod(textEdit_, "insertHtml", Qt::QueuedConnection,
               Q_ARG(QString, qstr));*/
        }
        catch (const std::exception&)
        {

        }
    

         emit logMessageAvailable(qstr);

    }

    void flush_() override {}
    signals:
        void logMessageAvailable(const QString& message);

  
private:
    QTextEdit* textEdit_;
       QTimer* timer_;
};

#endif // QTLOGSINK_H

