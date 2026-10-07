C***********************************************
      SUBROUTINE  PAGE( iou)
C***********************************************
      CALL TRACE ('PAGE    ')
      WRITE(iou,1)
    1 FORMAT(1H1)
c   1 FORMAT(1H)
      CALL TRACK ('PAGE    ')
      RETURN
      END
