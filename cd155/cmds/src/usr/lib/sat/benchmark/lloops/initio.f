C
C*************************************************
      SUBROUTINE INITIO( iou, name )
C***********************************************************************
C                                                                      *
C       INITIO - Assign logdevice nr "iou" to disk file "name"         *
C                                                                      *
C          iou - logical i/o device number                             *
C         name - name to assign to disk file                           *
C                                                                      *
C***********************************************************************
      LOGICAL LIVING
      CHARACTER *(*) name
C     CALL TRACE ('INITIO  ')
C
           INQUIRE( FILE=name, EXIST= LIVING )
                IF( LIVING ) THEN
                    OPEN ( UNIT=iou, FILE=name, STATUS='OLD')
                    CLOSE( UNIT=iou, STATUS='DELETE')
               ENDIF
             OPEN  (UNIT=iou, FILE=name, STATUS='NEW')
C
C     CALL TRACK ('INITIO  ')
      RETURN
      END
