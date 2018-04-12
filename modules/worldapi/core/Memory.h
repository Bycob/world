#ifndef WORLD_MEMORY_H
#define WORLD_MEMORY_H

#include "core/WorldConfig.h"

namespace world {

    template <typename T> class ConstRefOrValue;

    struct __prov__ {
    private:

        template <typename T>
        class IRefOrValue {
        public:
            virtual ~IRefOrValue() = default;

            virtual T &get() = 0;

            virtual IRefOrValue<T> *copy() = 0;
        };

        template <typename T>
        class IConstRefOrValue {
        public:
            virtual ~IConstRefOrValue() = default;

            virtual const T &get() const = 0;

            virtual IConstRefOrValue<T> *copy() const = 0;
        };

        template<typename T>
        class Ref : public IRefOrValue<T> {
        public:
            Ref(T &t)
                    : _ref(t) {}

            ~Ref() override = default;

            T &get() override {
                return _ref;
            }

            IRefOrValue<T> *copy() override {
                return new Ref<T>(_ref);
            }

        private:
            T &_ref;
        };

        template<typename T>
        class ConstRef : public IConstRefOrValue<T> {
        public:
            ConstRef(const T &t)
                    : _ref(t) {}

            ~ConstRef() override = default;

            virtual const T &get() const {
                return _ref;
            }

            virtual IConstRefOrValue<T> *copy() const {
                return new ConstRef<T>(_ref);
            }

        private:
            const T &_ref;
        };

        template<typename T>
        class Value : public IConstRefOrValue<T>, public IRefOrValue<T> {
        public:
            Value(const T &t)
                    : _value(t) {}

            ~Value() override = default;

            T &get() override {
                return _value;
            }

            const T &get() const override {
                return _value;
            }

            IConstRefOrValue<T> *copy() const override {
                return new Value<T>(_value);
            }

            IRefOrValue<T> *copy() override {
                return new Value<T>(_value);
            }

        private:
            T _value;
        };

        template <typename T>
        friend class ConstRefOrValue;
        template <typename T>
        friend class RefOrValue;
    };

    template <typename T>
    class RefOrValue {
    public:
        RefOrValue(const T &t)
                : _rov(new __prov__::Value<T>(t)){}

        RefOrValue(T &t, bool isRef) {
            if (isRef) {
                _rov = new __prov__::Ref<T>(t);
            }
            else {
                _rov = new __prov__::Value<T>(t);
            }
        }

        RefOrValue(const RefOrValue &other)
                : _rov(other._rov->copy()) {}

        RefOrValue(RefOrValue &&other)
                : _rov(other._rov) {
            other._rov = nullptr;
        }

        ~RefOrValue() {
            if (_rov != nullptr) {
                delete _rov;
            }
        }

        RefOrValue &operator=(const RefOrValue &other) {
            _rov = other._rov->copy();
            return *this;
        }

        RefOrValue &operator=(RefOrValue &&other) {
            if (_rov != nullptr) {
                delete _rov;
            }
            _rov = other._rov;
            other._rov = nullptr;
			return *this;
        }

        T &get() {
            return _rov->get();
        }

        const T &get() const {
            return _rov->get();
        }

        T &operator*() {
            return _rov->get();
        }

        const T &operator*() const {
            return _rov->get();
        }

        T *operator->() {
            return &_rov->get();
        }

        const T *operator->() const {
            return &_rov->get();
        }

    private:
        __prov__::IRefOrValue<T> *_rov;
    };

    template <typename T>
    class ConstRefOrValue {
    public:
        ConstRefOrValue(const T &t, bool isRef) {
            if (isRef) {
                _crov = new __prov__::ConstRef<T>(t);
            }
            else {
                _crov = new __prov__::Value<T>(t);
            }
        }

        ConstRefOrValue(const ConstRefOrValue &other)
                : _crov(other._crov->copy()) {}

        ConstRefOrValue(ConstRefOrValue &&other)
                : _crov(other._crov) {
            other._crov = nullptr;
        }

        ~ConstRefOrValue() {
            if (_crov != nullptr) {
                delete _crov;
            }
        }

        ConstRefOrValue &operator=(const ConstRefOrValue &other) {
            if (_crov != nullptr) {
                delete _crov;
            }
            _crov = other._crov->copy();
            return *this;
        }

        ConstRefOrValue &operator=(ConstRefOrValue &&other) {
            if (_crov != nullptr) {
                delete _crov;
            }
            _crov = other._crov;
            other._crov = nullptr;
			return *this;
        }

        const T &get() const {
            return _crov->get();
        }

        const T &operator*() const {
            return _crov->get();
        }

        const T *operator->() const {
            return &_crov->get();
        }
    private:
        __prov__::IConstRefOrValue<T> *_crov;
    };
}

#endif //WORLD_MEMORY_H
