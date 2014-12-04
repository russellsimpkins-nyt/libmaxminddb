%define maxmindSrc https://github.com/russellsimpkins-nyt/libmaxminddb.git

Summary: libmaxminddb provides functions for the maxmind geoip database
Name: libmaxmdinddb
Version: 1.0.0
Release: 1
License: Apache
Group: MaxMind
URL: https://github.com/maxmind/libmaxminddb
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
A library for using hte maxmind geoip database

%prep
rm -rf %{_builddir}/%{name}-%{version}-%{release}
git clone --recursive  %{maxmindSrc} %{_builddir}/%{name}-%{version}-%{release}

%build
cd %{name}-%{version}-%{release}
./bootstrap
%configure

%install
rm -rf $RPM_BUILD_ROOT
cd %{name}-%{version}-%{release}
make install DESTDIR=%{buildroot}

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%doc
/usr


%changelog
* Thu Dec  4 2014 root <root@vagrant-centos65.vagrantup.com> - 
- Initial build.

