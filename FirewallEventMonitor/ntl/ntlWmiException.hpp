// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See License.txt in the project root for license information.

#pragma once

// cpp headers
#include <wchar.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <exception>
#include <algorithm>
// os headers
#include <windows.h>
#include <Objbase.h>
#include <Wbemidl.h>
// ntl headers
#include "ntlException.hpp"
#include "ntlComInitialize.hpp"
#include "ntlVersionConversion.hpp"

namespace ntl
{

    ////////////////////////////////////////////////////////////////////////////////
    ///
    /// WmiErrorInfo
    ///
    /// class which encapsulates information about CIM_Error's that are returned
    ///   through *_async methods.
    /// 
    /// Each __ExtendedStatus and CIM_Error property is exposed though a method
    /// All properties return bool - if that property was returned or not.
    ///
    /// class __ExtendedStatus 
    /// {
    ///   string ProviderName;
    ///   string Operation;
    ///   string ParameterInfo;
    ///   string Description;
    ///   uint StatusCode; 
    /// };
    ///
    /// class CIM_Error
    /// {
    ///   uint16 ErrorType; xxx
    ///   string OtherErrorType;
    ///   string OwningEntity;
    ///   string MessageID;
    ///   string Message;
    ///   string MessageArguments[];
    ///   uint16 PerceivedSeverity;
    ///   uint16 ProbableCause;
    ///   string ProbableCauseDescription;
    ///   string RecommendedActions[];
    ///   string ErrorSource;
    ///   uint16 ErrorSourceFormat = 0;
    ///   string OtherErrorSourceFormat;
    ///   uint32 CIMStatusCode;
    ///   string CIMStatusCodeDescription;
    /// };
    /// 
    ////////////////////////////////////////////////////////////////////////////////
    class WmiErrorInfo {
    private:
        //
        // Marking mutable as the COM methods are not marked 'const'
        //   and 'Get' is the only COM interface invoked from a const method
        //
        mutable ntl::ComPtr<IWbemClassObject> error_info;

    public:
        // c'tors and d'tor
        WmiErrorInfo() NOEXCEPT : error_info(nullptr)
        {
            get_error_info();
        }

        explicit WmiErrorInfo(const ntl::ComPtr<IWbemClassObject>& _error_info) NOEXCEPT : error_info(_error_info)
        {
        }
        ~WmiErrorInfo() NOEXCEPT
        {
        }

        // copy c'tor and move c'tor
        WmiErrorInfo(const WmiErrorInfo& _inObj) NOEXCEPT : error_info(_inObj.error_info)
        {
        }
        // assignment operators with lvalues and rvalues
        WmiErrorInfo& operator=(const WmiErrorInfo& _inObj) NOEXCEPT
        {
            WmiErrorInfo temp(_inObj);
            this->swap(temp);
            return *this;
        }

        // move c'tors
        WmiErrorInfo(_In_ WmiErrorInfo&& _inObj) NOEXCEPT : error_info(nullptr)
        {
            this->swap(_inObj);
        }
        WmiErrorInfo& operator=(_In_ WmiErrorInfo&& _inObj) NOEXCEPT
        {
            WmiErrorInfo temp(std::move(_inObj));
            this->swap(temp);
            return *this;
        }

        void get_error_info() NOEXCEPT
        {
            ntl::ComPtr<IErrorInfo> errorInfo;
            HRESULT hr = ::GetErrorInfo(0, errorInfo.get_addr_of());
            if (S_OK == hr) {
                //  S_FALSE == no error object was returned
                hr = errorInfo->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>((this->error_info.get_addr_of())));
                if (SUCCEEDED(hr)) {
                    hr = S_OK; // ensure it's exactly S_OK...
                }
            }
            if (hr != S_OK) {
                this->error_info.release();
            }
        }

        void release() NOEXCEPT
        {
            this->error_info.release();
        }

        bool has_error() const NOEXCEPT
        {
            return (this->error_info.get() != nullptr);
        }

        std::wstring error_text() const
        {
            return (this->write_ExtendedStatus() + this->write_CIMError());
        }

        void swap(_Inout_ WmiErrorInfo& _lhs) NOEXCEPT
        {
            using std::swap;
            swap(this->error_info, _lhs.error_info);
        }

        //
        // All __ExtendedStatus properties exposed
        //
        _Success_(return) bool get_ProviderName(_Inout_ std::wstring* _provider_name) const
        {
            _provider_name->clear();
            return this->get_parameter_value(L"ProviderName", _provider_name);
        }

        _Success_(return) bool get_Operation(_Inout_ std::wstring* _operation) const
        {
            _operation->clear();
            return this->get_parameter_value(L"Operation", _operation);
        }

        _Success_(return) bool get_ParameterInfo(_Inout_ std::wstring* _parameter_info) const
        {
            _parameter_info->clear();
            return this->get_parameter_value(L"ParameterInfo", _parameter_info);
        }

        _Success_(return) bool get_Description(_Inout_ std::wstring* _description) const
        {
            _description->clear();
            return this->get_parameter_value(L"Description", _description);
        }

        _Success_(return) bool get_StatusCode(_Out_ unsigned int* _status_code) const
        {
            *_status_code = 0xffffffff;
            return this->get_parameter_value(L"StatusCode", _status_code);
        }
        //
        // All CIM_Error properties exposed
        //
        _Success_(return) bool get_ErrorType(_Out_ unsigned* _error_type) const
        {
            *_error_type = 0xffffffff;
            return this->get_parameter_value(L"ErrorType", _error_type);
        }

        _Success_(return) bool get_OtherErrorType(_Inout_ std::wstring* _other_error_type) const
        {
            _other_error_type->clear();
            return this->get_parameter_value(L"OtherErrorType", _other_error_type);
        }

        _Success_(return) bool get_OwningEntity(_Inout_ std::wstring* _owning_entity) const
        {
            _owning_entity->clear();
            return this->get_parameter_value(L"OwningEntity", _owning_entity);
        }

        _Success_(return) bool get_MessageID(_Inout_ std::wstring* _message_id) const
        {
            _message_id->clear();
            return this->get_parameter_value(L"MessageID", _message_id);
        }

        _Success_(return) bool get_Message(_Inout_ std::wstring* _message) const
        {
            _message->clear();
            return this->get_parameter_value(L"Message", _message);
        }

        _Success_(return) bool get_MessageArguments(_Inout_ std::vector<std::wstring>* _message_arguments) const
        {
            _message_arguments->clear();
            return this->get_parameter_value(L"MessageArguments", _message_arguments);
        }

        _Success_(return) bool get_PerceivedSeverity(_Out_ unsigned short* _perceived_severity) const
        {
            *_perceived_severity = 0xffff;
            return this->get_parameter_value(L"PerceivedSeverity", _perceived_severity);
        }

        _Success_(return) bool get_ProbableCause(_Out_ unsigned short* _probable_cause) const
        {
            *_probable_cause = 0xffff;
            return this->get_parameter_value(L"ProbableCause", _probable_cause);
        }

        _Success_(return) bool get_ProbableCauseDescription(_Inout_ std::wstring* _probable_cause_description) const
        {
            _probable_cause_description->clear();
            return this->get_parameter_value(L"ProbableCauseDescription", _probable_cause_description);
        }

        _Success_(return) bool get_RecommendedActions(_Inout_ std::vector<std::wstring>* _recommended_actions) const
        {
            _recommended_actions->clear();
            return this->get_parameter_value(L"RecommendedActions", _recommended_actions);
        }

        _Success_(return) bool get_ErrorSource(_Inout_ std::wstring* _error_source) const
        {
            _error_source->clear();
            return this->get_parameter_value(L"ErrorSource", _error_source);
        }

        _Success_(return) bool get_ErrorSourceFormat(_Out_ unsigned short* _error_source_format) const
        {
            *_error_source_format = 0xffff;
            return this->get_parameter_value(L"ErrorSourceFormat", _error_source_format);
        }

        _Success_(return) bool get_OtherErrorSourceFormat(_Inout_ std::wstring* _other_error_source_format) const
        {
            _other_error_source_format->clear();
            return this->get_parameter_value(L"OtherErrorSourceFormat", _other_error_source_format);
        }

        _Success_(return) bool get_CIMStatusCode(_Out_ unsigned int* _cim_status_code) const
        {
            *_cim_status_code = 0;
            return this->get_parameter_value(L"CIMStatusCode", _cim_status_code);
        }

        _Success_(return) bool get_CIMStatusCodeDescription(_Inout_ std::wstring* _cim_status_code_description) const
        {
            _cim_status_code_description->clear();
            return this->get_parameter_value(L"CIMStatusCodeDescription", _cim_status_code_description);
        }

        std::wstring write_ExtendedStatus() const;
        std::wstring write_CIMError() const;

    private:
        // common functions

        template <typename T>
        _Success_(return) bool get_parameter_value(_In_ LPCWSTR _parameter, _Out_ T* _value) const;
    };

    class WmiException : public ntl::Exception {
    public:
        ///
        /// constructors
        ///
        WmiException() NOEXCEPT : ntl::Exception(), className(nullptr), errorInfo()
        {
        }

        explicit WmiException(HRESULT _ulCode) :
            ntl::Exception(_ulCode),
            className(nullptr),
            errorInfo()
        {
        }
        explicit WmiException(HRESULT _ulCode, const IWbemClassObject* _classObject) :
            ntl::Exception(_ulCode),
            className(nullptr),
            errorInfo()
        {
            get_className(_classObject);
        }

        explicit WmiException(_In_ LPCWSTR _wszMessage, const bool _bMessageCopy = true) NOEXCEPT :
        ntl::Exception(_wszMessage, _bMessageCopy),
            className(nullptr),
            errorInfo()
        {
        }
        explicit WmiException(_In_ LPCWSTR _wszMessage, const IWbemClassObject* _classObject, const bool _bMessageCopy = true) NOEXCEPT :
        ntl::Exception(_wszMessage, _bMessageCopy),
            className(nullptr),
            errorInfo()
        {
            get_className(_classObject);
        }

        explicit WmiException(const HRESULT _ulCode, _In_ LPCWSTR _wszMessage, const bool _bMessageCopy = true) NOEXCEPT :
        ntl::Exception(_ulCode, _wszMessage, _bMessageCopy),
            className(nullptr),
            errorInfo()
        {
        }
        explicit WmiException(const HRESULT _ulCode, const IWbemClassObject* _classObject, _In_ LPCWSTR _wszMessage, const bool _bMessageCopy = true) NOEXCEPT :
        ntl::Exception(_ulCode, _wszMessage, _bMessageCopy),
            className(nullptr),
            errorInfo()
        {
            get_className(_classObject);
        }

        explicit WmiException(const HRESULT _ulCode, _In_ LPCWSTR _wszMessage, _In_ LPCWSTR _wszLocation, const bool _bBothStringCopy = true) NOEXCEPT :
        ntl::Exception(_ulCode, _wszMessage, _wszLocation, _bBothStringCopy),
            className(nullptr),
            errorInfo()
        {
        }
        explicit WmiException(const HRESULT _ulCode, const IWbemClassObject* _classObject, _In_ LPCWSTR _wszMessage, _In_ LPCWSTR _wszLocation, const bool _bBothStringCopy = true) NOEXCEPT :
        ntl::Exception(_ulCode, _wszMessage, _wszLocation, _bBothStringCopy),
            className(nullptr),
            errorInfo()
        {
            get_className(_classObject);
        }
        ///
        /// Copy c'tor
        ///
        WmiException(const WmiException& e) NOEXCEPT :
        ntl::Exception(e),
            className(nullptr),
            errorInfo(e.errorInfo)
        {
            // must do a deep copy, not just copy the ptr
            // if this fails, we'll just have nullptr instead of a class name
            // - not failing this just because we can't get extra info
            if (e.className != nullptr) {
                className = ::SysAllocString(e.className);
            }
        }

        ///
        /// destructor
        ///
        virtual ~WmiException() NOEXCEPT
        {
            // SysFreeString handles NULL
            ::SysFreeString(className);
        }

        // public accessors
        virtual const wchar_t* class_w() const NOEXCEPT
        {
            return (className == nullptr) ? L"" : className;
        }

        virtual WmiErrorInfo error_info() const NOEXCEPT
        {
            // copy c'tor of WmiErrorInfo is no-throw
            // - so is safe to return a copy here
            return errorInfo;
        }

    private:
        // using a raw BSTR to ensure no method will throw
        BSTR className;
        WmiErrorInfo errorInfo;

        void get_className(const IWbemClassObject* _classObject) NOEXCEPT
        {
            //
            // protect against a null IWbemClassObject pointer
            //
            if (_classObject != nullptr) {
                VARIANT variant;
                ::VariantInit(&variant);
                // the method should allow to be called from const() methods
                // - forced to const-cast to make this const-correct
                HRESULT hr = const_cast<IWbemClassObject*>(_classObject)->Get(
                    L"__CLASS", 0, &variant, nullptr, nullptr);
                if (SUCCEEDED(hr)) {
                    // copy the BSTR from the VARIANT
                    // - do NOT free the VARIANT
                    this->className = variant.bstrVal;
                }
            }
        }
    };


    inline std::wstring WmiErrorInfo::write_ExtendedStatus() const
    {
        /// class __ExtendedStatus 
        /// {
        ///   string ProviderName;
        ///   string Operation;
        ///   string ParameterInfo;
        ///   string Description;
        ///   uint StatusCode; 
        /// };
        std::wstring wsTemp;
        unsigned int intTemp;
        wchar_t iString[32];

        std::wstring wsprint(L"__ExtendedStatus information:");

        if (this->get_parameter_value(L"ProviderName", &wsTemp)) {
            wsprint.append(L"\n\tProviderName: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"Operation", &wsTemp)) {
            wsprint.append(L"\n\tOperation: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"ParameterInfo", &wsTemp)) {
            wsprint.append(L"\n\tParameterInfo: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"Description", &wsTemp)) {
            wsprint.append(L"\n\tDescription: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"StatusCode", &intTemp)) {
            if (0 == _itow_s(intTemp, iString, 10)) {
                iString[31] = L'\0';
                wsprint.append(L"\n\tStatusCode: ");
                wsprint.append(iString);
            }
        }

        wsprint.push_back(L'\n');
        return wsprint;
    }

    inline std::wstring WmiErrorInfo::write_CIMError() const
    {
        /// class CIM_Error
        /// {
        ///   uint16 ErrorType;
        ///   string OtherErrorType;
        ///   string OwningEntity;
        ///   string MessageID;
        ///   string Message;
        ///   string MessageArguments[];
        ///   uint16 PerceivedSeverity;
        ///   uint16 ProbableCause;
        ///   string ProbableCauseDescription;
        ///   string RecommendedActions[];
        ///   string ErrorSource;
        ///   uint16 ErrorSourceFormat = 0;
        ///   string OtherErrorSourceFormat;
        ///   uint32 CIMStatusCode;
        ///   string CIMStatusCodeDescription;
        /// };
        std::wstring wsTemp;
        unsigned int intTemp;
        unsigned short shortTemp;
        wchar_t iString[32];
        std::vector<std::wstring> vsTemp;

        std::wstring wsprint(L"CIMError information:");

        if (this->get_parameter_value(L"ErrorType", &intTemp)) {
            if (0 == _itow_s(intTemp, iString, 10)) {
                iString[31] = L'\0';
                wsprint.append(L"\n\tErrorType: ");
                wsprint.append(iString);
            }
        }

        if (this->get_parameter_value(L"OtherErrorType", &wsTemp)) {
            wsprint.append(L"\n\tOtherErrorType: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"OwningEntity", &wsTemp)) {
            wsprint.append(L"\n\tOwningEntity: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"MessageID", &wsTemp)) {
            wsprint.append(L"\n\tMessageID: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"Message", &wsTemp)) {
            wsprint.append(L"\n\tMessage: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"MessageArguments", &vsTemp)) {
            wsprint.append(L"\n\tMessageArguments:");
            std::for_each(vsTemp.begin(), vsTemp.end(), [&] (const std::wstring& wsMessage) {
                wsprint.append(L"\n\t");
                wsprint.append(wsMessage);
            });
        }

        if (this->get_parameter_value(L"PerceivedSeverity", &shortTemp)) {
            if (0 == _itow_s(shortTemp, iString, 10)) {
                iString[31] = L'\0';
                wsprint.append(L"\n\tPerceivedSeverity: ");
                wsprint.append(iString);
            }
        }

        if (this->get_parameter_value(L"ProbableCause", &shortTemp)) {
            if (0 == _itow_s(shortTemp, iString, 10)) {
                iString[31] = L'\0';
                wsprint.append(L"\n\tProbableCause: ");
                wsprint.append(iString);
            }
        }

        if (this->get_parameter_value(L"ProbableCauseDescription", &wsTemp)) {
            wsprint.append(L"\n\tProbableCauseDescription: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"RecommendedActions", &vsTemp)) {
            wsprint.append(L"\n\tRecommendedActions:");
            std::for_each(vsTemp.begin(), vsTemp.end(), [&] (const std::wstring& wsAction) {
                wsprint.append(L"\n\t");
                wsprint.append(wsAction);
            });
        }

        if (this->get_parameter_value(L"ErrorSource", &wsTemp)) {
            wsprint.append(L"\n\tErrorSource: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"ErrorSourceFormat", &shortTemp)) {
            if (0 == _itow_s(shortTemp, iString, 10)) {
                iString[31] = L'\0';
                wsprint.append(L"\n\tErrorSourceFormat: ");
                wsprint.append(iString);
            }
        }

        if (this->get_parameter_value(L"OtherErrorSourceFormat", &wsTemp)) {
            wsprint.append(L"\n\tOtherErrorSourceFormat: ");
            wsprint.append(wsTemp);
        }

        if (this->get_parameter_value(L"CIMStatusCode", &intTemp)) {
            if (0 == _itow_s(intTemp, iString, 10)) {
                iString[31] = L'\0';
                wsprint.append(L"\n\tCIMStatusCode: ");
                wsprint.append(iString);
            }
        }

        if (this->get_parameter_value(L"CIMStatusCodeDescription", &wsTemp)) {
            wsprint.append(L"\n\tCIMStatusCodeDescription: ");
            wsprint.append(wsTemp);
        }

        wsprint.push_back(L'\n');
        return wsprint;
    }

    template <typename T>
    _Success_(return) bool WmiErrorInfo::get_parameter_value(_In_ LPCWSTR _parameter, _Out_ T* _value) const
    {
        ntl::ComVariant var;
        if (this->error_info.get() != nullptr) {
            HRESULT hr = this->error_info->Get(_parameter, 0, var.get(), nullptr, nullptr);
            if (FAILED(hr)) {
                if (hr != WBEM_E_NOT_FOUND) {
                    wchar_t function[128];
                    ::_snwprintf_s(function, 128, _TRUNCATE, L"WmiErrorInfo::%s", _parameter);
                    throw WmiException(hr, this->error_info.get(), L"IWbemClassObject::Get", function, false);
                }
            }
        }
        if (!var.is_empty() && !var.is_null()) {
            // suppressing the 'Using uninitialized memory' warning since T* must be an _Out_ 
            // but retrieve can be _Inout_. This is an artifact in how retrieve works with 
            // both POD (_Out_) and non-POD (_Inout_) types
#pragma warning( suppress : 6001)
            var.retrieve(_value);
            return true;
        } else {
            return false;
        }
    }

    //
    // non-member swap function
    //
    inline void swap(_Inout_ WmiErrorInfo& _rhs, _Inout_ WmiErrorInfo& _lhs) NOEXCEPT
    {
        _rhs.swap(_lhs);
    }
} // namespace
