#include "replace.h"
#include <QString>
#include <QDebug>

Replace::Replace(QString content)
{
    m_content = content;
}


/**
 * @brief 判断关键字是否有效
 * @param templateName
 * @return
 */
bool Replace::templateValid(QString templateName)
{
    if(templateName.isEmpty())
    {
        return false;
    }
    return true;
}

/**
 * @brief 获取当前内容中所有的关键字
 * @return
 */
QStringList Replace::currTemplate()
{

    QStringList currTemplate;                                   //创建一个字符串列表对象
    QString templateName;
    int start,end;
    for(int i=0;i<m_content.size();++i)
    {
        if(m_content.mid(i,2) == QString("$(") )                //判断是否为指定字符串$(
        {
            start = m_content.indexOf("$(",i);                  //指定字符串$(的下标
            end = m_content.indexOf(")",i);                     //指定字符串)的下标
            templateName = m_content.mid(start+2,end-start-2);  //关键字
            if(templateValid(templateName))
            {
                if(!currTemplate.contains(templateName))        //若关键字不存在，则加入列表
                {
                    currTemplate << templateName;               //加入字符串列表
                }
            }

        }
    }
    return currTemplate;  //返回列表
}

/**
 * @brief 对获取内容的关键字进行替换
 */
QString Replace::replaceTemplate()
{
    QStringList list = currTemplate();      //获取关键字的列表
    QString content;
    qDebug()<< list;

    for(int i=0;i<list.size();++i)
    {
        content = m_content.replace("$("+list.at(i)+")","replaced");      //替换内容中那个的关键字

    }
    return content;
}

/**
 * @brief 添加关键字和对应的值
 * @param key
 * @param value
 */
bool Replace::addTemplateValue(QString templateName, QString value)
{
    if(!m_map_template.contains(templateName))
    {
        m_map_template.insert(templateName,value);
        return true;
    }
    return false;
}


/**
 * @brief Replace::replace
 * @param templateName
 * @param filaPath 文件路径
 * @return
 */
QString Replace::replace(QString filePath)
{

}
