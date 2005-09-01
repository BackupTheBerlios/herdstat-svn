/*
 * herdstat -- src/saxparser.cc
 * $Id$
 * Copyright (c) 2005 Aaron Walker <ka0ttic@gentoo.org>
 *
 * This file is part of herdstat.
 *
 * herdstat is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * herdstat is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * herdstat; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 325, Boston, MA  02111-1257  USA
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "saxparser.hh"

#include <utility>
#include <cstdlib>
#include <cstdarg>

#include <libxml/parserInternals.h> /* for xmlCreateFileParserCtxt() */

SAXParser::init SAXParser::_init;

namespace {
    extern "C" void xml_err(void *ctx, const char *fmt, ...);
} // anonymous namespace

struct SAXCallback
{
    static void start_document(void *ctx);
    static void end_document(void *ctx);
    static void start_element(void *ctx, const xmlChar *name, const xmlChar **p);
    static void end_element(void *ctx, const xmlChar *name);
    static void characters(void *ctx, const xmlChar *text, int len);
    static void comment(void *ctx, const xmlChar *text);
    static void warning(void *ctx, const char *fmt, ...);
    static void error(void *ctx, const char *fmt, ...);
    static void fatal_error(void *ctx, const char *fmt, ...);
    static void cdata(void *ctx, const xmlChar *text, int len);
};

SAXParser::init::init()
{
    /* keep libxml2 from using stderr */
    xmlSetGenericErrorFunc(0, xml_err);
    xmlInitParser();
}

SAXParser::init::~init()
{
    xmlCleanupParser();
}

SAXParser::SAXParser() : _context(NULL), _handler(new xmlSAXHandler)
{
    std::memset(_handler.get(), 0, sizeof(xmlSAXHandler));
    _handler->startDocument = SAXCallback::start_document;
    _handler->endDocument   = SAXCallback::end_document;
    _handler->startElement  = SAXCallback::start_element;
    _handler->endElement    = SAXCallback::end_element;
    _handler->characters    = SAXCallback::characters;
    _handler->comment       = SAXCallback::comment;
    _handler->warning       = SAXCallback::warning;
    _handler->error         = SAXCallback::error;
    _handler->fatalError    = SAXCallback::fatal_error;
    _handler->cdataBlock    = SAXCallback::cdata;
}

SAXParser::~SAXParser()
{
    destroy_context();
}

void
SAXParser::init_context()
{
    assert(_context);
    _context->linenumbers = 1;
//    _context->validate = (_validate ? 1 : 0);
    _context->_private = this;
}

void
SAXParser::destroy_context()
{
    if (_context)
    {
        _context->_private = NULL;

        if (_context->myDoc)
            xmlFreeDoc(_context->myDoc);

        xmlFreeParserCtxt(_context);
        _context = NULL;
    }
}

void
SAXParser::start_document()
{
}

void
SAXParser::end_document()
{
}

void
SAXParser::start_element(const std::string &name, const attrs_type &attrs)
{
}

void
SAXParser::end_element(const std::string &name)
{
}

void
SAXParser::characters(const std::string &text)
{
}

void
SAXParser::comment(const std::string &text)
{
}

void
SAXParser::warning(const std::string &text)
{
}

void
SAXParser::error(const std::string &text)
{
}

void
SAXParser::fatal_error(const std::string &text)
{
    throw SAXParser_E("Fatal: " + text);
}

void
SAXParser::cdata(const std::string &text)
{
}

void
SAXParser::parse()
{
    assert(_context);

    xmlSAXHandler *old = _context->sax;
    _context->sax = _handler.get();

    init_context();

    xmlParseDocument(_context);

    _context->sax = old;

    if (not _context->wellFormed)
        throw SAXParser_E("not well formed");

    destroy_context();
}

void
SAXParser::parse_file(const std::string &path)
{
    if (_context)
        throw SAXParser_E("Attempting to instantiate a second parser context");

    _context = xmlCreateFileParserCtxt(path.c_str());
    parse();
}

void
SAXCallback::start_document(void *ctx)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

//    try
//    {
        parser->start_document();
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::end_document(void *ctx)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

//    try
//    {
        parser->end_document();
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::start_element(void *ctx, const xmlChar *text, const xmlChar **p)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

    SAXParser::attrs_type attrs;
    if (p)
    {
        for (const xmlChar **cur = p ; cur and *cur ; cur += 2)
            attrs.insert(std::make_pair((char *)*cur, (char *)*(cur + 1)));
    }

//    try
//    {
        parser->start_element(std::string((const char *)text), attrs);
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::end_element(void *ctx, const xmlChar *name)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

//    try
//    {
        parser->end_element(std::string((const char *)name));
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::characters(void *ctx, const xmlChar *text, int len)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

//    try
//    {
        parser->characters(
            std::string(
                reinterpret_cast<const char *>(text),
                reinterpret_cast<const char *>(text + len)));
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::comment(void *ctx, const xmlChar *text)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

//    try
//    {
        parser->comment(std::string((const char *)text));
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::warning(void *ctx, const char *fmt, ...)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

    va_list v;
    char buf[1024];

    va_start(v, fmt);
    vsnprintf(buf, sizeof(buf)/sizeof(buf[0]), fmt, v);
    va_end(v);

//    try
//    {
        parser->warning(buf);
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::error(void *ctx, const char *fmt, ...)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

    va_list v;
    char buf[1024];

    va_start(v, fmt);
    vsnprintf(buf, sizeof(buf)/sizeof(buf[0]), fmt, v);
    va_end(v);

//    try
//    {
        parser->error(buf);
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::fatal_error(void *ctx, const char *fmt, ...)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

    va_list v;
    char buf[1024];

    va_start(v, fmt);
    vsnprintf(buf, sizeof(buf)/sizeof(buf[0]), fmt, v);
    va_end(v);

//    try
//    {
        parser->fatal_error(buf);
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

void
SAXCallback::cdata(void *ctx, const xmlChar *text, int len)
{
    xmlParserCtxt *context = static_cast<xmlParserCtxt*>(ctx);
    SAXParser *parser = static_cast<SAXParser*>(context->_private);

//    try
//    {
        parser->cdata(
            std::string(
                reinterpret_cast<const char *>(text),
                reinterpret_cast<const char *>(text + len)));
//    }
//    catch (const SAXParser_E &e)
//    {

//    }
}

namespace {
    extern "C" void xml_err(void *, const char *, ...) { /* ... */ }
} // anonymous namespace

/* vim: set tw=80 sw=4 et : */
