Summary: icm
Name: icm
Version: 1.1
Release: 1
License: GPL
Group: Library
Source: icm-1.1.tar.gz
#BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root
BuildRoot:      %{_topdir}/BUILDROOT

%description
icm libs and message borker

%prep
rm -fr %{buildroot}
%setup -q

%build
make -j4

%install
mkdir -p %{buildroot}/opt/icm/lib
mkdir -p %{buildroot}/opt/icm/bin
mkdir -p %{buildroot}/opt/icm/conf
mkdir -p %{buildroot}/opt/icm/log
cp %{_builddir}/%{name}-%{version}/lib/*.so %{buildroot}/opt/icm/lib
cp %{_builddir}/%{name}-%{version}/bin/icmmsg %{buildroot}/opt/icm/bin
cp %{_builddir}/%{name}-%{version}/conf/icmmsg.ini %{buildroot}/opt/icm/conf

%post
have=`grep -w /opt/icm/lib /etc/ld.so.conf.d/icm.conf 2>/dev/null`
if [ -z $have ]; then
    echo /opt/icm/lib >> /etc/ld.so.conf.d/icm.conf
    ldconfig
fi


%files
/opt/icm/lib/*.so
/opt/icm/bin/icmmsg
/opt/icm/conf/icmmsg.ini
/opt/icm/log

%changelog
* Mon Mar 12 2012 robert
+ create package

