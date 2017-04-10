from remotedb_factory import RemoteDatabaseFactory
import sys

def ldapSearch(term, subject, catalog):
    course_db = RemoteDatabaseFactory.build('ualberta')
    ldap_return = course_db.searchForAllSections(term, subject, catalog)
    print(ldap_return)


ldapSearch(sys.argv[1], sys.argv[2], sys.argv[3])